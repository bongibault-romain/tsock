/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
/* pour la gestion des string */
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "tsock.h"

int main(int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1;	 /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int length_message = 30; /* Taille des messages à envoyer ou à recevoir, par défaut : 30 */
	int tcp = 1;			 /* 0=UDP, 1=TCP */
	int multiple = 0;		 /* 0=un seul client, 1=plusieurs clients */

	int emitter = -1; // 0 = emetteur des données, 1 = récepteur des données
	int client = -1;  // 0 = serveur de la connexion, 1 = client de la connexion

	while ((c = getopt(argc, argv, "n:ul:mecrs")) != -1)
	{
		switch (c)
		{
		case 'e':
			if (emitter != -1)
			{
				usage();
				fprintf(stderr, "(-e,-r) erreur: vous avez déjà spécifié si vous êtes l'émetteur ou le récepteur.\n");
				exit(1);
			}

			emitter = 1;
			break;
		case 'c':
			if (client != -1)
			{
				usage();
				fprintf(stderr, "(-c,-s) erreur: vous avez déjà spécifié si vous êtes le client ou le serveur.\n");
				exit(1);
			}

			client = 1;
			break;
		case 'r':
			if (emitter != -1)
			{
				usage();
				fprintf(stderr, "(-e,-r) erreur: vous avez déjà spécifié si vous êtes l'émetteur ou le récepteur.\n");
				exit(1);
			}

			emitter = 0;
			break;
		case 's':
			if (client != -1)
			{
				usage();
				fprintf(stderr, "(-c,-s) erreur: vous avez déjà spécifié si vous êtes le client ou le serveur.\n");
				exit(1);
			}

			client = 0;
			break;
		case 'l':
			length_message = atoi(optarg);

			if (length_message <= 0)
			{
				usage();
				exit(1);
			}

			break;
		case 'n':
			nb_message = atoi(optarg);
			break;
		case 'u':
			tcp = 0;
			break;
		case 'm':
			multiple = 1;
			break;
		default:
			usage();
			break;
		}
	}

	if (emitter == -1)
	{
		usage();
		fprintf(stderr, "(-e,-r) erreur: vous devez spécifier si vous êtes l'émetteur ou le récepteur.\n");
		exit(1);
	}

	if (client == -1)
	{
		usage();
		fprintf(stderr, "(-c,-s) erreur: vous devez spécifier si vous êtes le client ou le serveur.\n");
		exit(1);
	}

	if (nb_message == -1 && emitter == 1)
	{
		nb_message = 10;
	}

	if (multiple == 1 && client == 1)
	{
		fprintf(stderr, "(-m,-s) erreur: seul le serveur peut accepter plusieurs connexions.\n");
		exit(1);
	}

	if (tcp == 0)
	{
		if (emitter == 1 && client == 0)
		{
			fprintf(stderr, "(-u,-s,-e) erreur: le serveur ne peut pas envoyer de message en UDP.\n");
			exit(1);
		}

		if (emitter == 0 && client == 1)
		{
			fprintf(stderr, "(-u,-c,-r) erreur: le client ne peut pas recevoir de message en UDP.\n");
			exit(1);
		}
	}

	// création d'un masque pour contrer l'action par défaut de SIGPIPE
	// ce signal est envoyé lorsqu'on tente d'écrire dans un socket fermé
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGPIPE);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	if (client == 1)
	{
		if (argc - optind != 2)
		{
			usage();
			fprintf(stderr, "erreur: vous devez spécifier l'hôte et le port de destination.\n");
			exit(1);
		}

		char *dest_host = argv[optind];
		int dest_port = atoi(argv[optind + 1]);

		if (nb_message == -1)
		{
			printf("[0] CLIENT : host=%s,port=%d,tcp=%d,emitter=%d,message_length=%d,message_amount=infinite\n", dest_host, dest_port, tcp, emitter, length_message);
		}
		else
		{
			printf("[0] CLIENT : host=%s,port=%d,tcp=%d,emitter=%d,message_length=%d,message_amount=%d\n", dest_host, dest_port, tcp, emitter, length_message, nb_message);
		}

		tsock_client(emitter, nb_message, dest_host, dest_port, length_message, tcp);
	}
	else
	{
		if (argc - optind != 1)
		{
			usage();
			fprintf(stderr, "erreur: vous devez spécifier le port sur lequel le serveur doit écouter.\n");
			exit(1);
		}

		int server_port = atoi(argv[optind]);

		if (nb_message == -1)
		{
			printf("[0] SERVEUR : port=%d,tcp=%d,emitter=%d,multiple_connections=%d,message_length=%d,message_amount=infinite\n", server_port, tcp, emitter, multiple, length_message);
		}
		else
		{
			printf("[0] SERVEUR : port=%d,tcp=%d,emitter=%d,multiple_connections=%d,message_length=%d,message_amount=%d\n", server_port, tcp, emitter, multiple, length_message, nb_message);
		}

		tsock_server(emitter, nb_message, server_port, length_message, tcp, multiple);
	}

	return EXIT_SUCCESS;
}

/*
 * Fonction qui permet de créer un serveur en fonction des paramètres passés
 */
void tsock_server(int emitter, int nb_message, int server_port, int length_message, int tcp, int multiple)
{
	if (tcp == 0)
	{
		server_udp(nb_message, server_port, length_message);
	}
	else if (tcp == 1)
	{
		server_tcp(emitter, nb_message, server_port, length_message, multiple);
	}
}

/*
 * Fonction qui permet de créer un client en fonction des paramètres passés
 */
void tsock_client(int emitter, int nb_message, char *dest_host, int dest_port, int length_message, int tcp)
{
	if (tcp == 0)
	{
		client_udp(nb_message, dest_host, dest_port, length_message);
	}
	else if (tcp == 1)
	{
		client_tcp(emitter, nb_message, dest_host, dest_port, length_message);
	}
}

/*
 * Fonction qui permet d'initialiser un serveur,
 * initialiser un socket et le lier à une adresse
 *
 * @param sock : le socket à initialiser
 * @param type : le type de socket à initialiser (SOCK_DGRAM ou SOCK_STREAM)
 * @param port : le port sur lequel le serveur doit écouter
 */
void initialize_server(int *sock, int type, int port)
{
	// création de la socket
	*sock = socket(AF_INET, type, 0);

	if (*sock == -1)
	{
		fprintf(stderr, "[0] SERVEUR - Impossible de créer la socket.\n");
		exit(1);
	}

	// initialisation de l'adresse du serveur
	struct sockaddr_in server_addr = {0};
	memset((char *)&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// liaison de la socket à l'adresse
	if (bind(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		fprintf(stderr, "[0] SERVEUR : Impossible de réserver le port.\n");
		exit(1);
	}
}

/*
 * Fonction qui permet d'initialiser un client,
 * initialiser un socket et le lier à une adresse
 *
 * @param sock : le socket à initialiser
 * @param dest_addr : l'adresse de destination où le client doit se connecter
 * @param type : le type de socket à initialiser (SOCK_DGRAM ou SOCK_STREAM)
 * @param dest_host : l'hôte de destination
 * @param dest_port : le port de destination
 */
void initialize_client(int *sock, struct sockaddr_in *dest_addr, int type, char *dest_host, int dest_port)
{
	// création de la socket
	*sock = socket(AF_INET, type, 0);
	if (*sock == -1)
	{
		printf("[0] CLIENT : Impossible de créer la socket.\n");
		exit(1);
	}

	// récupération de l'adresse ip de l'hôte
	struct hostent *add = gethostbyname(dest_host);
	if (add == NULL)
	{
		printf("[0] CLIENT : Impossible de récuperer l'adresse de cette hôte.\n");
		exit(1);
	}

	// initialisation de l'adresse de destination
	memset((char *)dest_addr, 0, sizeof(*dest_addr));
	(*dest_addr).sin_family = AF_INET;
	(*dest_addr).sin_port = htons(dest_port);
	memcpy((char *)&((*dest_addr).sin_addr.s_addr), add->h_addr_list[0], add->h_length);
}

/*
 * Fonction qui permet d'envoyer des messages via un socket UDP
 *
 * @param sock : le socket à utiliser pour l'envoi
 * @param dest_addr : l'adresse de destination où envoyer les messages
 * @param nb_message : le nombre de messages à envoyer
 * @param length_message : la taille des messages à envoyer
 */
void emitter_udp(int sock, struct sockaddr_in dest_addr, int nb_message, int length_message)
{
	char message[length_message];

	for (int i = 0; i < nb_message; i++)
	{
		construire_message(message, length_message, i);

		printf("[0] EMETTEUR : Envoie n°");
		affichage_message(message, length_message, i);

		sendto(sock, &message, length_message, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));
	}
}

/*
 * Fonction qui permet de recevoir des messages via un socket UDP
 *
 * @param sock : le socket à utiliser pour la réception
 * @param nb_message : le nombre de messages à recevoir
 * @param length_message : la taille des messages à recevoir
 */
void receiver_udp(int sock, int nb_message, int length_message)
{
	int received_messages = 0;
	char buffer[length_message];

	while ((received_messages < nb_message) || nb_message == -1)
	{
		int received = recvfrom(sock, buffer, length_message, 0, NULL, NULL);

		if (received == -1)
		{
			fprintf(stderr, "[0] RECEPTEUR : Erreur lors de la réception du message n°%d.\n", received_messages);
		}

		printf("[0] RECEPTEUR : Réception n°");
		affichage_message(buffer, length_message, received_messages);

		received_messages++;
	}
}

/*
 * Création d'un serveur utilisant le protocole UDP
 *
 * @param nb_message : le nombre de messages à recevoir
 * @param server_port : le port sur lequel le serveur doit écouter
 * @param length_message : la taille des messages à recevoir
 */
void server_udp(int nb_message, int server_port, int length_message)
{
	int sock;
	initialize_server(&sock, SOCK_DGRAM, server_port);

	receiver_udp(sock, nb_message, length_message);

	close(sock);
	printf("[0] SERVEUR : Fin de la réception de messages.\n");
}

/*
 * Création d'un client utilisant le protocole UDP
 *
 * @param nb_message : le nombre de messages à envoyer
 * @param dest_host : l'hôte de destination
 * @param dest_port : le port de destination
 * @param length_message : la taille des messages à envoyer
 */
void client_udp(int nb_message, char *dest_host, int dest_port, int length_message)
{
	int sock;
	struct sockaddr_in dest_addr;
	initialize_client(&sock, &dest_addr, SOCK_DGRAM, dest_host, dest_port);

	emitter_udp(sock, dest_addr, nb_message, length_message);

	close(sock);
	printf("[0] CLIENT : Fin de l'émission.\n");
}

/*
 * Fonction qui permet de construire un message
 *
 * @param message : le message à construire
 * @param lg : la taille du message à construire
 * @param no_msg : le numéro du message à construire
 */
void construire_message(char *message, int lg, int no_msg)
{
	// construction du motif (calcul pour obtenir une lettre de l'alphabet)
	char motif = ('a' + no_msg - 97) % 26 + 'a';

	for (int i = 0; i < lg; i++)
	{
		message[i] = motif;
	}

	// ajout du numéro du message au début du message sur 5 caractères
	int amount = sprintf(message, "%5d", no_msg);
	message[amount] = motif;
}

/*
 * Fonction qui permet d'envoyer des messages via un socket TCP
 *
 * @param sock : le socket à utiliser pour l'envoi
 * @param nb_message : le nombre de messages à envoyer
 * @param length_message : la taille des messages à envoyer
 */
void emitter_tcp(int sock, int nb_message, int length_message, unsigned int id)
{
	printf("[%u] EMETTEUR : Début de l'émission. Envoi de %d messages.\n", id, nb_message);

	char message[length_message];

	for (int i = 0; i < nb_message; i++)
	{
		construire_message(message, length_message, i);

		printf("[%u] EMETTEUR : Envoie n°", id);
		affichage_message(message, length_message, i);

		int written = write(sock, message, length_message);

		if (written == -1)
		{
			fprintf(stderr, "[%u] EMETTEUR - Erreur lors de l'envoi du message n°%d.\n", id, i);

			// Si le récepteur ferme la connexion, on sort de la boucle
			if (errno == EPIPE)
			{
				printf("[%u] EMETTEUR : Connexion fermée par le récepteur.\n", id);
				break;
			}
		}
	}

	printf("[%u] EMETTEUR : Fin de l'émission.\n", id);
}

/*
 * Fonction qui permet de recevoir des messages via un socket TCP
 *
 * @param sock : le socket à utiliser pour la réception
 * @param add_client : l'adresse du client qui envoie les messages
 * @param nb_message : le nombre de messages à recevoir
 * @param length_message : la taille des messages à recevoir
 */
void receiver_tcp(int sock, struct sockaddr_in add_client, int nb_message, int length_message, unsigned int id)
{
	char buffer[length_message];
	int received_messages = 0;

	if (nb_message == -1)
	{
		printf("[%u] RECEPTEUR : Début de la réception. Réception de messages jusqu'à la fermeture de la connexion.\n", id);
	}
	else
	{
		printf("[%u] RECEPTEUR : Début de la réception. Réception de %d messages.\n", id, nb_message);
	}

	// tant que le nombre de messages reçus est inférieur au nombre de messages à recevoir
	while ((received_messages < nb_message) || nb_message == -1)
	{
		int received = read(sock, buffer, length_message);

		if (received == -1)
		{
			fprintf(stderr, "[%u] RECEPTEUR : Erreur lors de la réception du message n°%d.\n", id, received_messages);
			break;
		}

		if (received == 0)
		{
			printf("[%u] RECEPTEUR : Connexion fermée par l'emetteur.\n", id);
			break;
		}

		if (received < 0)
		{
			continue;
		}

		/*
		 * Récupération de l'adresse IP du client
		 * Conversion de l'adresse IP en une chaîne de caractères
		 */
		char client_address[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(add_client.sin_addr), client_address, INET_ADDRSTRLEN);

		printf("[%u] RECEPTEUR : (%s) Réception n°", id, client_address);
		affichage_message(buffer, length_message, received_messages);

		received_messages++;
	}
}

/*
 * Fonction qui permet de créer un client utilisant le protocole TCP
 *
 * @param emitter : 0 = émetteur, 1 = récepteur
 * @param nb_message : le nombre de messages à envoyer ou à recevoir
 * @param dest_host : l'hôte de destination
 * @param dest_port : le port de destination
 * @param length_message : la taille des messages à envoyer ou à recevoir
 */
void client_tcp(int emitter, int nb_message, char *dest_host, int dest_port, int length_message)
{
	int sock;
	struct sockaddr_in dest_addr;

	initialize_client(&sock, &dest_addr, SOCK_STREAM, dest_host, dest_port);

	printf("[0] CLIENT : Demande de connexion.\n");

	if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "[0] CLIENT : Impossible d'établir la connexion.\n");
		exit(1);
	}

	printf("[0] CLIENT : Connexion établie.\n");

	if (emitter == 1)
	{
		emitter_tcp(sock, nb_message, length_message, 0);
	}
	else
	{
		receiver_tcp(sock, dest_addr, nb_message, length_message, 0);
	}

	printf("[0] CLIENT : Fermerture de la connexion.\n");

	shutdown(sock, SHUT_RDWR);
	close(sock);
}

/*
 * Fonction qui permet de créer un serveur utilisant le protocole TCP
 *
 * @param emitter : 0 = émetteur, 1 = récepteur
 * @param nb_message : le nombre de messages à envoyer ou à recevoir
 * @param server_port : le port sur lequel le serveur doit écouter
 * @param length_message : la taille des messages à envoyer ou à recevoir
 * @param multiple : 0 = un seul client, 1 = plusieurs clients
 */
void server_tcp(int emitter, int nb_message, int server_port, int length_message, int multiple)
{
	int sock;
	initialize_server(&sock, SOCK_STREAM, server_port);

	int nb_max = 20;

	if (listen(sock, nb_max) == -1)
	{
		fprintf(stderr, "[0] SERVEUR : Impossible de mettre à jour la file d'attente.\n");
		exit(1);
	}

	int amountAccepted = 0;
	unsigned int nextForkId = 1;

	while (amountAccepted < 1 || multiple == 1)
	{
		if (nextForkId == 0)
			nextForkId = 1;

		printf("[0] SERVEUR : En attente de connexion.\n");

		struct sockaddr_in add_client;
		unsigned int lg_add_client = 0;
		int socket_bis;

		if ((socket_bis = accept(sock, (struct sockaddr *)&add_client, &lg_add_client)) == -1)
		{
			fprintf(stderr, "[0] SERVEUR : Impossible d'accepter la connexion.\n");
			continue;
		}

		printf("[0] SERVEUR : Connexion acceptée.\n");

		amountAccepted++;

		switch (fork())
		{
		case -1:
			shutdown(socket_bis, SHUT_RDWR);
			close(socket_bis);
			fprintf(stderr, "[0] SERVEUR : Erreur lors du fork.\n");
			break;
		case 0:

			printf("[%u] SERVEUR : Connexion établie.\n", nextForkId);

			if (emitter == 1)
			{
				emitter_tcp(socket_bis, nb_message, length_message, nextForkId);
			}
			else
			{
				receiver_tcp(socket_bis, add_client, nb_message, length_message, nextForkId);
			}

			shutdown(socket_bis, SHUT_RDWR);
			close(socket_bis);
			exit(1);

			break;
		default:
			break;
		}

		nextForkId++;
	}

	printf("[0] SERVEUR : Fin de la réception de messages.\n");

	close(sock);

	printf("[0] SERVEUR : Attente de fermeture de connexion ouvertes.\n");

	// attente de la fin des fils
	wait(NULL);

	printf("[0] SERVEUR : Fin de la fermeture de connexion ouvertes.\n");
}

/*
 * Fonction qui permet d'afficher un message
 *
 * @param message : le message à afficher
 * @param lg : la taille du message à afficher
 * @param no_msg : le numéro du message à afficher
 */
void affichage_message(char *message, int lg, int no_msg)
{
	char msgft[lg + 1];
	memcpy(msgft, message, lg);
	msgft[lg] = '\0';
	printf("%d - [%s]\n", no_msg, msgft);
}

/*
 * Fonction qui permet d'afficher l'utilisation du programme
 */
void usage()
{
	printf("usage: cmd [-c|-s][-e|-r][-n ##][-l ##][-m]\n");
}