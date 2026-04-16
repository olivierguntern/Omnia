"""
QThread worker that runs the detection pipeline off the main thread.

Signals emitted to the GUI thread:
    frame_ready(np.ndarray)          annotated BGR frame
    speed_logged(dict)               one speed record
    stats_updated(dict)              aggregate stats
    error(str)                       error message
    finished()                       pipeline ended
"""

from __future__ import annotations

import time
from typing import Optional

import cv2
import numpy as np
from PySide6.QtCore import QThread, Signal

from ..config import CameraConfig, DetectionConfig, SpeedConfig
from ..camera_geometry import CameraGeometry
from ..detector import VehicleDetector
from ..tracker import MultiObjectTracker
from ..speed_estimator import SpeedEstimator
from ..visualizer import Visualizer
from ..logger import SpeedLogger


class DetectionWorker(QThread):
    frame_ready   = Signal(np.ndarray)
    speed_logged  = Signal(dict)
    stats_updated = Signal(dict)
    error         = Signal(str)
    finished      = Signal()

    def __init__(self, cam_cfg, det_cfg, spd_cfg, source, csv_path=None, parent=None):
        super().__init__(parent)
        self.cam_cfg  = cam_cfg
        self.det_cfg  = det_cfg
        self.spd_cfg  = spd_cfg
        self.source   = source
        self.csv_path = csv_path
        self._running = False
        self._paused  = False
        self.logger: Optional[SpeedLogger] = None

    def stop(self):   self._running = False
    def pause(self):  self._paused  = True
    def resume(self): self._paused  = False

    def run(self):
        try:
            self._run_pipeline()
        except Exception as exc:
            self.error.emit(str(exc))
        finally:
            self.finished.emit()

    def _run_pipeline(self):
        try:
            geo = CameraGeometry(self.cam_cfg)
        except Exception as exc:
            self.error.emit(f"Camera geometry error: {exc}")
            return

        detector  = VehicleDetector(self.det_cfg)
        tracker   = MultiObjectTracker(geo, max_age_frames=self.spd_cfg.max_track_age_frames)
        estimator = SpeedEstimator(self.spd_cfg)
        viz       = Visualizer(self.cam_cfg, self.spd_cfg)

        if self.csv_path:
            self.logger = SpeedLogger(self.csv_path)

        try:
            idx = int(self.source)
            cap = cv2.VideoCapture(idx)
        except ValueError:
            cap = cv2.VideoCapture(self.source)

        if not cap.isOpened():
            self.error.emit(f"Cannot open source: {self.source}")
            return

        cap.set(cv2.CAP_PROP_FRAME_WIDTH,  self.cam_cfg.image_width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_cfg.image_height)

        self._running = True
        t_start = time.monotonic()
        fps_times: list = []
        seen_speeds: dict = {}

        while self._running:
            if self._paused:
                time.sleep(0.05)
                continue

            ret, frame = cap.read()
            if not ret:
                break

            frame = cv2.resize(frame, (self.cam_cfg.image_width, self.cam_cfg.image_height))
            ts    = time.monotonic()

            detections = detector.detect(frame)
            tracks     = tracker.update(detections, ts)

            for track in tracks:
                speed = estimator.update_track_speed(track)
                if speed is not None and speed != seen_speeds.get(track.track_id):
                    seen_speeds[track.track_id] = speed
                    direction = estimator.direction_deg(track)
                    wp = list(track.history)[-1] if track.history else None
                    record = {
                        "timestamp_s":   round(ts - t_start, 3),
                        "track_id":      track.track_id,
                        "speed_kmh":     speed,
                        "direction_deg": round(direction, 1) if direction else None,
                        "world_x_m":     round(wp.world_x, 3) if wp else None,
                        "world_y_m":     round(wp.world_y, 3) if wp else None,
                    }
                    self.speed_logged.emit(record)
                    if self.logger:
                        self.logger.log(record["timestamp_s"], track.track_id, speed,
                                        direction, wp.world_x if wp else 0.0,
                                        wp.world_y if wp else 0.0)

            annotated = viz.draw(frame.copy(), tracks)
            self.frame_ready.emit(annotated)

            fps_times.append(ts)
            if len(fps_times) > 30: fps_times.pop(0)
            dt  = fps_times[-1] - fps_times[0] if len(fps_times) > 1 else 1e-6
            fps = (len(fps_times) - 1) / dt if dt > 0 else 0.0
            self.stats_updated.emit({
                "fps": round(fps, 1),
                "track_count": len(tracks),
                "elapsed_s": round(ts - t_start, 1),
            })

        cap.release()
