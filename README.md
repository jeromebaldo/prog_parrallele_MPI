# PROGRAMMATION MPI

Travail réalisée dans le cadre du ma dernière année en baccalauréat informatique. 

## OBJECTIF:

L'application est un programme parallèle permettant de calculer toutes les racines d'une fonction à l'intérieur d'un intervalle spécifié par l'utilisateur. 
Le projet utilise MPI (Message Interface Passing) pour réaliser cette manipulation. 

## ETAPES POUR LE FONCTIONNEMENT DE MPI:

### Etape 01: 
installation de la version v10.1.2 de  MPI en utilisant le lien suivant :
https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi

### Etape 02:
L'utilisateur doit d'abord créer l'exécutable de l'application.
Ensuite en se positionnant dans le dossier de l'exécutable crée, l'utilisateur doit taper la commande dans le terminal:
- mpiexec -n 8 nomFichierAExécuter 

explication sur la commande:

- mpiexec spécifie que l'exécutable sera exécuté avec MPI 

- -n 8 spécifie le nombre total de processus mis en parallèle pour l'application 

- nomFichierAExécuter : nom du fichier à exécuter

### Précision sur la commande 

L’application peut-être lancé sans arguments supplémentaires. Il runnera en utilisant l’équation par défaut donnée dans l’énoncé du laboratoire. Si l’utilisateur souhaite paramétrer les coefficients pour la résolution de racines, il doit taper l’argument en ligne de commande de cette façon : 
mpiexec -n “nombreprocessus”  .\exemple-envoi-vecteur.exe 0.1-0.5-0.9-5.3-8.0-

Il faudra toujours penser à mettre entre tirets les coefficients et finir par un tiret. 

### Interval analysé
L’interval est hardcodé dans l’application. Sauf  modification direct, l’interval sera toujours de 0 à 10 compris. 

### Précision
La précision est réalisée par l’instanciation de deux constantes:
- #define NBRVIRGUL 5  
- #define DECALAGE 0.00001 => comprend l’incrémentation de la valeur dans chaque interval. NBRVIRGUL est la précision du résultat.

## DESCRIPTION DE L'APPLICATION:

### structure 
Hormis pour le processus 0, tous les processus sont esclaves. Ils calculent dans leur interval puis affichent les résultats avant d’envoyer au processus maitre ainsi que le nombre de racines trouvées.  
Le processus 0 est le processus maitre qui calcule son interval et le nombre de racines trouvées. Il collecte ensuite toutes les racines envoyées par les autres processus. Pour finir il affiche l’ensemble des résultats compilés et triés avec le nombre total de racines trouvées.

Les fonctionnalités utilisées sont le MPI_Recv et MPI_Send permettant de recevoir et envoyer les données. 


 
 
