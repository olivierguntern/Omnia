// Fichier: test.omn
// Ce programme teste la syntaxe complète du langage Omnia

// --- MODULES & IMPORTS ---
import math
import utils as u

// --- CONSTANTES ---
const PI: Float = 3.14159

// --- VARIABLES ---
let x = 10             // variable immuable
let y: Int = 20        // variable immuable avec type explicite
var counter = 0        // variable mutable

// --- DÉCLARATION DE FONCTIONS SIMPLES ---
fun add(a: Int, b: Int) -> Int:
{
    return a + b
}

fun greet(name: String = "Invité") -> String:
{
    return "Bonjour, " + name
}

fun variadic_sum(*args: Int) -> Int:
{
    let total = 0
    for num in args:
        total += num
    return total
}

// --- FONCTION LAMBDA ---
let multiply = fun(x: Int, y: Int) -> Int: x * y

// --- FONCTIONS UTILITAIRES (map, filter, reduce) ---
fun map(list: List<Int>, f: fun(Int) -> Int) -> List<Int>:
{
    let result = []
    for item in list:
        result.push(f(item))
    return result
}

fun filter(list: List<Int>, condition: fun(Int) -> Bool) -> List<Int>:
{
    let result = []
    for item in list:
        if condition(item):
            result.push(item)
    return result
}

fun reduce(list: List<Int>, initial: Int, f: fun(Int, Int) -> Int) -> Int:
{
    let result = initial
    for item in list:
        result = f(result, item)
    return result
}

// --- COMPREHENSIONS --- 
let squares = [ x * x for x in 1..10 if x % 2 == 0 ]
let mapping = { str(x): x for x in 1..10 if x > 5 }

// --- EXPRESSIONS CONDITIONNELLES ---
let status = if x > y: "x est supérieur" else: "x est inférieur ou égal"

// --- BOUCLES ---
fun count_to(n: Int) -> Unit:
{
    let i = 0
    while i < n:
    {
        print("i = " + str(i))
        i = i + 1
    }
}

// --- GESTION DES ERREURS ---
fun safe_divide(a: Float, b: Float) -> Float:
{
    try:
    {
        return a / b
    }
    catch e:
    {
        print("Erreur: " + e.message)
        return 0
    }
    finally:
    {
        print("Opération de division terminée")
    }
}

// --- CONCURRENCE AVEC ACTEURS ---
struct Actor:
{
    messages: List<String>
    
    fun send(self, message: String):
    {
        self.messages.push(message)
    }
    
    fun receive(self) -> String:
    {
        if self.messages.size() > 0:
            return self.messages.pop()
        else:
            return "Aucun message"
    }
    
    fun run(self):
    {
        while self.messages.size() > 0:
        {
            let msg = self.receive()
            print("Message reçu: " + msg)
        }
    }
}

fun test_actor() -> Unit:
{
    let actor = Actor { messages: [] }
    actor.send("Message 1")
    actor.send("Message 2")
    actor.run()
}

// --- CONCURRENCE AVEC THREADS ---
fun parallel_task(id: Int) -> String:
{
    return "Tâche " + str(id) + " terminée"
}

fun test_threads() -> Unit:
{
    let t1 = spawn parallel_task(1)
    let t2 = spawn parallel_task(2)
    let r1 = t1.join()
    let r2 = t2.join()
    print("Thread 1: " + r1)
    print("Thread 2: " + r2)
}

// --- POO : CLASSES, TRAITS ET HÉRITAGE ---
// Classe simple et fonction membre
struct Point:
{
    public x: Int
    public y: Int
    
    fun norm(self) -> Float:
    {
        return sqrt(self.x * self.x + self.y * self.y)
    }
}

// Trait (interface)
trait Speakable:
{
    fun speak(self) -> String
}

// Classe de base
struct Animal:
{
    protected name: String
    
    fun speak(self) -> String:
    {
        return "L'animal fait un bruit"
    }
}

// Classe dérivée implémentant un trait
struct Dog inherits Animal, Speakable:
{
    private breed: String
    
    override fun speak(self) -> String:
    {
        return "Le chien " + self.name + " aboie: Woof!"
    }
}

// --- TEST DES FONCTIONNALITÉS COMPLETES DANS main() ---
fun main():
{
    print("=== Test Variables et Opérations ===")
    print("x = " + str(x) + ", y = " + str(y))
    print("Somme (x+y): " + str(add(x, y)))
    print("Produit via lambda (x * y): " + str(multiply(x, y)))
    print("Variadic sum (1,2,3): " + str(variadic_sum(1, 2, 3)))
    
    print("=== Test Fonctions utilitaires ===")
    let numbers: List<Int> = [1, 2, 3, 4, 5]
    let doubled = map(numbers, fun(n: Int) -> Int: n * 2)
    let evens = filter(numbers, fun(n: Int) -> Bool: n % 2 == 0)
    let total = reduce(numbers, 0, fun(acc: Int, n: Int) -> Int: acc + n)
    print("Doubled: " + str(doubled))
    print("Evens: " + str(evens))
    print("Total: " + str(total))
    
    print("=== Test Comprehensions ===")
    print("Squares: " + str(squares))
    print("Mapping: " + str(mapping))
    
    print("=== Test Expression conditionnelle ===")
    print("Status: " + status)
    
    print("=== Boucle while (count_to) ===")
    count_to(5)
    
    print("=== Test Try/Catch/Finally ===")
    print("Division (10/2): " + str(safe_divide(10.0, 2.0)))
    print("Division (10/0): " + str(safe_divide(10.0, 0.0)))
    
    print("=== Test Acteurs (Concurrence) ===")
    test_actor()
    
    print("=== Test Threads ===")
    test_threads()
    
    print("=== Test Programmation Orientée Objet ===")
    let dog = Dog { name: "Rex", breed: "Berger Allemand" }
    print(dog.speak())
    let point = Point { x: 3, y: 4 }
    print("Norme du point: " + str(point.norm()))
    
    print("=== Test Expression Lambda (fonction anonyme) ===")
    let adder = fun(a: Int, b: Int) -> Int: a + b
    print("adder(5,7): " + str(adder(5, 7)))
}
