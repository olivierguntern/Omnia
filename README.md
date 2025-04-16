# Omnia


Un interpréteur complet du langage Omnia écrit en Python..
Ce projet implémente une vaste partie de la syntaxe d’Omnia, incluant la gestion des variables (mutables et immuables), des fonctions (y compris les lambdas), des structures de contrôle (conditions, boucles, try/catch/finally), des littéraux (listes, dictionnaires), ainsi que des constructions de la programmation orientée objet et de la concurrence (acteurs, spawn de threads).


---

Table des matières

Caractéristiques

Installation

Utilisation

Exemples

Contribuer

Licence



---

Caractéristiques

Variables et Mutabilité
Gère des variables déclarées avec let (immuable), var (mutable) et const (immuable) avec contrôle de la réaffectation.

Fonctions et Lambda
Déclaration de fonctions classiques avec paramètres par défaut et variadiques, ainsi que des fonctions lambda.

Structures de Contrôle
Support complet des instructions conditionnelles (if, else, elif), boucles (while et itérations sur des collections) et expressions conditionnelles.

Littéraux et Compréhensions
Prise en charge des littéraux numériques, chaînes de caractères, listes (avec comprehensions) et dictionnaires.

Gestion des Exceptions
Implémentation de blocs try/catch/finally pour un contrôle précis des erreurs durant l'exécution.

Programmation Orientée Objet
Support des déclarations de classes, héritage, et utilisation de traits/interfaces pour une structure d’objet avancée.

Concurrence
Modélisation des acteurs et support de threads pour le parallélisme.



---

Installation

Prérequis

Python 3.6 ou supérieur

(Optionnel) Environnement Linux (par exemple via UserLAnd sur Android) ou tout système compatible avec Python


Installation en ligne de commande

Clonez ce dépôt et installez les dépendances (si nécessaire) :

git clone https://github.com/votre-utilisateur/omnia-interpreter.git
cd omnia-interpreter

Pour préparer l’environnement Python et installer l’interpréteur, vous pouvez exécuter le script fourni :

chmod +x install_omnia.sh
./install_omnia.sh

Ce script met à jour les paquets, installe Python3 et pip, crée un environnement virtuel, et génère le fichier omnia_interpreter.py.


---

Utilisation

Pour lancer l'interpréteur Omnia, assurez-vous d’activer votre environnement virtuel (si utilisé), puis exécutez :

source omnia_env/bin/activate
./omnia_interpreter.py chemin/vers/votre_fichier.omn

Le fichier source Omnia (.omn) peut contenir toutes les constructions syntaxiques supportées par le langage. Un exemple de fichier de test est fourni dans le dépôt pour illustrer diverses fonctionnalités.


---

Exemples

Voici un extrait d’un fichier Omnia de test (test.omn) qui explore un maximum de la syntaxe :

// Exemple de fichier test.omn

import math
import utils as u

const PI: Float = 3.14159

let x = 10          // Variable immuable
var y: Int = 20     // Variable mutable

fun add(a: Int, b: Int) -> Int:
{
    return a + b
}

let greet = fun(name: String = "Invité") -> String: "Bonjour, " + name

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
        print("Division terminée")
    }
}

struct Dog:
{
    protected name: String
    fun speak(self) -> String:
    {
        return "Le chien " + self.name + " aboie : Woof!"
    }
}

fun main():
{
    print("Addition: " + str(add(x, y)))
    print(greet("Alice"))
    print("Division sécurisée: " + str(safe_divide(10.0, 2.0)))
    
    let myDog = Dog { name: "Rex" }
    print(myDog.speak())
}


---

Contribuer

Les contributions sont les bienvenues !
Si vous souhaitez améliorer l’interpréteur, ajouter de nouvelles fonctionnalités ou corriger des bugs, n’hésitez pas à créer une pull request ou ouvrir une issue.


---

Licence

Ce projet est sous licence MIT.


---

Ce projet est une base de travail pour le langage Omnia et continue d’évoluer afin d’intégrer des fonctionnalités avancées et des optimisations. Si vous avez des suggestions ou des demandes spécifiques, merci de les partager dans la section des issues du dépôt.


---

Ce fichier README.md fournit une vue d’ensemble complète et structurée du projet, facilitant la prise en main et l’utilisation de l’interpréteur Omnia.
N’hésitez pas à l’adapter au fur et à mesure de l’évolution du projet !

