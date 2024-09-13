// #define _POSIX_C_SOURCE 200809L
// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "utils.h"


int main(int argc, char *argv[]) {
        if (argc != 3) {
	    fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
	    return 1;
	}

	char *server_ip = argv[1];
	int port = atoi(argv[2]);
	if (port <= 0) {
            fprintf(stderr, "Please provide a valid port number.\n");
	    return 1;
	}

	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[MAX_BUFFER_SIZE] = {0};
	fd_set readfds;
	char *logoutMsg = "/logout\n";

	// Create the socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	// Convert IPv4 addresses from text to binary form
	if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	// Connect to the server
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	while(1) {
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		FD_SET(0, &readfds); // 0 is the file descriptor for stdin

		select(sock+1, &readfds, NULL, NULL, NULL);

		memset(buffer, 0, MAX_BUFFER_SIZE);
		if (FD_ISSET(sock, &readfds)) { // Message from server
			valread = read(sock, buffer, MAX_BUFFER_SIZE);
			if(valread > 0) {
				printf("%s", buffer);


				// Check if the message is the logout confirmation
				if (startsWith(buffer, "Bye!! Have a nice day") == 0) {
					break; // Exit loop to close the connection
				}

			}

			fflush(stdout);
			memset(buffer, 0, MAX_BUFFER_SIZE);
		}

		if (FD_ISSET(0, &readfds)) { // Message to send
			fgets(buffer, 1024, stdin);
			send(sock, buffer, strlen(buffer), 0);

			// Check if the message is the logout command
			if (strcmp(buffer, logoutMsg) == 0) {
				continue; // Wait for server response before closing
			}

		}
	}

	close(sock);
	return 0;
}
