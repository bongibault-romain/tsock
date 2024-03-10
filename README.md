
# TSock

L’application à mettre en œuvre est nommée `tsock` et doit permettre de réaliser des échanges d'informations entre deux machines connectées à l’Internet. `tsock` est configurable soit comme une source d'informations (un émetteur), soit comme un puits d'informations (récepteur).

## Groupe

- Dalquier Léo
- Bongibault Romain


## Installation

Compilation du projet à l'aide de `make` ou de `gcc` :

```bash
$ make clean
$ make all
```

```
$ gcc -Wall -Wextra -Iinclude src/tsock.c -o tsock
```
    
## Fonctionnalités

- prise en compte de l’option `-u` basée sur l'utilisation du service fourni par UDP.
- par défaut `tsock` utilise le protocol TCP.
- prise en compte de l'option `-c` (client) ou `-s` (serveur) pour déterminer qui sera le client de la connexion.
- prise en compte de l'option `-e` (émetteur) ou `-r` (récepteur) pour déterminer qui sera l'émetteur/récepteur des données.
- prise en compte de l'option `-n valeur` pour déterminer le nombre de message à émettre/recevoir.
- prise en compte de l'option `-l valeur` pour déterminer la longueur des messages à émettre/recevoir.
- prise en compte de l'option `-m` (uniquement si l'option `-s` est présente et uniquement avec le protocol TCP) pour déterminer si plusieurs connexion seront admises ou non.

**Note:** certaines combinaisons d'option ne sont pas disponible avec UDP (`-u`). Si cette option est activée le **serveur** devra être obligatoirement **récepteur** et le **client** devra être **émetteur**.

## Utilisation

L'usage général de l'application tsock doit être le suivant :

Pour la mise en œuvre d'un client se connectant à un serveur s'exécutant sur la station `host` en attente sur le port `port`.

```bash
$ tsock -c [-options] host port
```

Pour la mise en œuvre d'un serveur en attente sur le port `port` :


```bash
$ tsock -s [-options] port
```

## Exemple d'utilisation

Lancement de `tsock` en utilisant le protocol UDP en mode client et émetteur :

```bash
$ tsock -u -c -e localhost 25565 
[0] CLIENT : host=localhost,port=25565,tcp=0,emitter=1,message_length=30,message_amount=10
[0] EMETTEUR : Envoie n°0 - [    0aaaaaaaaaaaaaaaaaaaaaaaaa]
[0] EMETTEUR : Envoie n°1 - [    1bbbbbbbbbbbbbbbbbbbbbbbbb]
[0] EMETTEUR : Envoie n°2 - [    2ccccccccccccccccccccccccc]
[0] EMETTEUR : Envoie n°3 - [    3ddddddddddddddddddddddddd]
[0] EMETTEUR : Envoie n°4 - [    4eeeeeeeeeeeeeeeeeeeeeeeee]
[0] EMETTEUR : Envoie n°5 - [    5fffffffffffffffffffffffff]
[0] EMETTEUR : Envoie n°6 - [    6ggggggggggggggggggggggggg]
[0] EMETTEUR : Envoie n°7 - [    7hhhhhhhhhhhhhhhhhhhhhhhhh]
[0] EMETTEUR : Envoie n°8 - [    8iiiiiiiiiiiiiiiiiiiiiiiii]
[0] EMETTEUR : Envoie n°9 - [    9jjjjjjjjjjjjjjjjjjjjjjjjj]
[0] CLIENT : Fin de l'émission.
```

Lancement de `tsock` en utilisant le protocol UDP en mode serveur et récepteur :

```bash
$ tsock -u -s -r 25565
[0] SERVEUR : port=25565,tcp=0,emitter=0,multiple_connections=0,message_length=30,message_amount=infinite
[0] RECEPTEUR : Réception n°0 - [    0aaaaaaaaaaaaaaaaaaaaaaaaa]
[0] RECEPTEUR : Réception n°1 - [    1bbbbbbbbbbbbbbbbbbbbbbbbb]
[0] RECEPTEUR : Réception n°2 - [    2ccccccccccccccccccccccccc]
[0] RECEPTEUR : Réception n°3 - [    3ddddddddddddddddddddddddd]
[0] RECEPTEUR : Réception n°4 - [    4eeeeeeeeeeeeeeeeeeeeeeeee]
[0] RECEPTEUR : Réception n°5 - [    5fffffffffffffffffffffffff]
[0] RECEPTEUR : Réception n°6 - [    6ggggggggggggggggggggggggg]
[0] RECEPTEUR : Réception n°7 - [    7hhhhhhhhhhhhhhhhhhhhhhhhh]
[0] RECEPTEUR : Réception n°8 - [    8iiiiiiiiiiiiiiiiiiiiiiiii]
[0] RECEPTEUR : Réception n°9 - [    9jjjjjjjjjjjjjjjjjjjjjjjjj]
```

Lancement de `tsock` en utilisant le protocol TCP en mode client et émetteur :

```bash
$ tsock -c -e localhost 25565 
[0] CLIENT : host=localhost,port=25565,tcp=1,emitter=1,message_length=30,message_amount=10
[0] CLIENT : Demande de connexion.
[0] CLIENT : Connexion établie.
[0] EMETTEUR : Début de l'émission. Envoi de 10 messages.
[0] EMETTEUR : Envoie n°0 - [    0aaaaaaaaaaaaaaaaaaaaaaaaa]
[0] EMETTEUR : Envoie n°1 - [    1bbbbbbbbbbbbbbbbbbbbbbbbb]
[0] EMETTEUR : Envoie n°2 - [    2ccccccccccccccccccccccccc]
[0] EMETTEUR : Envoie n°3 - [    3ddddddddddddddddddddddddd]
[0] EMETTEUR : Envoie n°4 - [    4eeeeeeeeeeeeeeeeeeeeeeeee]
[0] EMETTEUR : Envoie n°5 - [    5fffffffffffffffffffffffff]
[0] EMETTEUR : Envoie n°6 - [    6ggggggggggggggggggggggggg]
[0] EMETTEUR : Envoie n°7 - [    7hhhhhhhhhhhhhhhhhhhhhhhhh]
[0] EMETTEUR : Envoie n°8 - [    8iiiiiiiiiiiiiiiiiiiiiiiii]
[0] EMETTEUR : Envoie n°9 - [    9jjjjjjjjjjjjjjjjjjjjjjjjj]
[0] EMETTEUR : Fin de l'émission.
[0] CLIENT : Fermerture de la connexion.
```

Lancement de `tsock` en utilisant le protocol TCP en mode serveur et récepteur :

```bash
$ tsock -s -r 25565
[0] SERVEUR : port=25565,tcp=1,emitter=0,multiple_connections=0,message_length=30,message_amount=infinite
[0] SERVEUR : En attente de connexion.
[0] SERVEUR : Connexion acceptée.
[0] SERVEUR : Fin de la réception de messages.
[0] SERVEUR : Attente de fermeture de connexion ouvertes.
[1] SERVEUR : Connexion établie.
[1] RECEPTEUR : Début de la réception. Réception de messages jusqu'à la fermeture de la connexion.
[1] RECEPTEUR : (0.0.0.0) Réception n°0 - [    0aaaaaaaaaaaaaaaaaaaaaaaaa]
[1] RECEPTEUR : (0.0.0.0) Réception n°1 - [    1bbbbbbbbbbbbbbbbbbbbbbbbb]
[1] RECEPTEUR : (0.0.0.0) Réception n°2 - [    2ccccccccccccccccccccccccc]
[1] RECEPTEUR : (0.0.0.0) Réception n°3 - [    3ddddddddddddddddddddddddd]
[1] RECEPTEUR : (0.0.0.0) Réception n°4 - [    4eeeeeeeeeeeeeeeeeeeeeeeee]
[1] RECEPTEUR : (0.0.0.0) Réception n°5 - [    5fffffffffffffffffffffffff]
[1] RECEPTEUR : (0.0.0.0) Réception n°6 - [    6ggggggggggggggggggggggggg]
[1] RECEPTEUR : (0.0.0.0) Réception n°7 - [    7hhhhhhhhhhhhhhhhhhhhhhhhh]
[1] RECEPTEUR : (0.0.0.0) Réception n°8 - [    8iiiiiiiiiiiiiiiiiiiiiiiii]
[1] RECEPTEUR : (0.0.0.0) Réception n°9 - [    9jjjjjjjjjjjjjjjjjjjjjjjjj]
[1] RECEPTEUR : Connexion fermée par l'emetteur.
[0] SERVEUR : Fin de la fermeture de connexion ouvertes.
```