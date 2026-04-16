"""
Thread-safe CSV logger for speed detection events.

Each row captures one speed measurement for one track:
    timestamp_s, track_id, speed_kmh, direction_deg, world_x, world_y
"""

from __future__ import annotations

import csv
import io
import threading
from pathlib import Path
from typing import List, Optional


_FIELDS = [
    "timestamp_s",
    "track_id",
    "speed_kmh",
    "direction_deg",
    "world_x_m",
    "world_y_m",
]


class SpeedLogger:
    """
    Appends speed records to a CSV file in a thread-safe manner.

    Parameters
    ----------
    path : str | Path
        Destination CSV file.  Created (with header) on first write.
    """

    def __init__(self, path: str | Path) -> None:
        self._path = Path(path)
        self._lock = threading.Lock()
        self._rows: List[dict] = []
        self._file_initialised = False

    def log(
        self,
        timestamp_s: float,
        track_id: int,
        speed_kmh: float,
        direction_deg: Optional[float],
        world_x: float,
        world_y: float,
    ) -> None:
        row = {
            "timestamp_s": round(timestamp_s, 3),
            "track_id": track_id,
            "speed_kmh": round(speed_kmh, 1),
            "direction_deg": round(direction_deg, 1) if direction_deg is not None else "",
            "world_x_m": round(world_x, 3),
            "world_y_m": round(world_y, 3),
        }
        with self._lock:
            self._rows.append(row)
            self._write_row(row)

    def flush(self) -> None:
        pass

    def all_rows(self) -> List[dict]:
        with self._lock:
            return list(self._rows)

    def to_csv_string(self) -> str:
        buf = io.StringIO()
        writer = csv.DictWriter(buf, fieldnames=_FIELDS)
        writer.writeheader()
        with self._lock:
            writer.writerows(self._rows)
        return buf.getvalue()

    def clear(self) -> None:
        with self._lock:
            self._rows.clear()

    def _write_row(self, row: dict) -> None:
        write_header = not self._file_initialised and not self._path.exists()
        with self._path.open("a", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=_FIELDS)
            if write_header:
                writer.writeheader()
            writer.writerow(row)
        self._file_initialised = True
