void tsock_server(int emitter, int nb_message, int server_port, int length_message, int tcp, int multiple);

void tsock_client(int emitter, int nb_message, char *dest_host, int dest_port, int length_message, int tcp);

void client_udp(int nb_message, char *dest_host, int dest_port, int length_message);

void server_udp(int nb_message, int server_port, int length_message);

void construire_message(char *message, int lg, int no_msg);

void affichage_message(char *message, int lg, int no_msg);

void client_tcp(int emitter, int nb_message, char *dest_host, int dest_port, int length_message);

void server_tcp(int emitter, int nb_message, int server_port, int length_message, int multiple);

void emitter_udp(int sock, struct sockaddr_in dest_addr, int nb_message, int length_message);

void receiver_udp(int sock, int nb_message, int length_message);

void emitter_tcp(int sock, int nb_message, int length_message, unsigned int id);

void receiver_tcp(int sock, struct sockaddr_in add_client, int nb_message, int length_message, unsigned int id);

void usage();