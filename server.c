// #define _POSIX_C_SOURCE 200809L
// #define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h> // For select()
#include <errno.h>
#include <ctype.h>
#include "uuid4.h"
#include "utils.h"

#define TRUE 1
#define FALSE 0


//define a message struct containing from to and message
typedef struct {
	char *from;
	char *to;
	char *message;
} Message;


void sendActiveClients(int *pInt, char pString[10][UUID4_STR_BUFFER_SIZE], int i);

Message **
sendMessageToClient(int *usersIds, char userIds[10][UUID4_STR_BUFFER_SIZE], char *string, int i, Message **chatHistory);

char *getBotReply(BotMessage **messages, char buffer[1025]);


Message **addMessageToChatHistory(Message **history, char *string, char string1[37], char *string2);

bool isReceiverSender(Message *ptr, char string[37], char *string1);

char *getGptBotReply(char buffer[1025]);

char *getGptReply(const char *str);

int main(int argc, char *argv[]) {
        if (argc != 3) {
	    fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
	    return 1;
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);
	if (port <= 0) {
            fprintf(stderr, "Please provide a valid port number.\n");
	    return 1;
	}

	printf("Configured to listen on %s:%d\n", ip, port);
	
	// Run the setup script
	int result = system("chmod +x setup_venv.sh");

	if (result != 0) {
		printf("Failed to set up the Python environment.\n");
		return 1;
	}else{
		printf("Python environment set up successfully\n");
	}

	result = system("./setup_venv.sh");
	if (result != 0) {
		// Handle error
		printf("Failed to set up the Python environment.\n");
		return 1;
	} else {
		printf("Python environment set up successfully\n");
	}

        

	UUID4_STATE_T state;
	UUID4_T uuid;

	uuid4_seed(&state);

	//get array of bot messages
	BotMessage **botMessages = loadBotMessages();

	Message **chatHistory = NULL;

	int opt = TRUE;
	int master_socket, addrlen, new_socket, client_socket[10], max_clients = 10, activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;
	//one client_uuid is of size UUID4_STR_BUFFER_SIZE
	char client_uuids[10][UUID4_STR_BUFFER_SIZE];
	bool botEnabled[10];
	bool botV2Enabled[10];

	for (int j = 0; j < 10; ++j) {
		client_uuids[j][0] = '\0';
		botEnabled[j] = false;
		botV2Enabled[j] = false;
	}


	char buffer[1025];  // Data buffer for incoming messages

	// Set of socket descriptors
	fd_set readfds;

	// Initialize all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}



	// Create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set master socket to allow multiple connections
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// Type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);;
	address.sin_port = htons(port);

	// Bind the socket to localhost port
	if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", port);

	// Try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// Accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while (TRUE) {
		// Clear the socket set
		FD_ZERO(&readfds);

		// Add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		// Add child sockets to set
		for (i = 0; i < max_clients; i++) {
			// Socket descriptor
			sd = client_socket[i];

			// If valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);

			// Highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		// Wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			printf("select error");
		}

		// If something happened on the master socket, then it's an incoming connection
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			// Inform user of socket number - used in send and receive commands
			printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr),
				   ntohs(address.sin_port));

			// Add new socket to array of sockets
			for (i = 0; i < max_clients; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;

					uuid4_gen(&state, &uuid);
					uuid4_to_s(uuid, client_uuids[i], UUID4_STR_BUFFER_SIZE);
					printf("Adding to list of sockets as %d\n", i);
					char message[MAX_BUFFER_SIZE] = "\t\tWelcome to the chat server! \nYour UUID is: ";
					char *val = strcat(message, client_uuids[i]);
					val = strcat(val, "\n\nuser> ");
					send(new_socket, val, strlen(val), 0);
					break;
				}
			}
		}

		// Else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds)) {
				memset(buffer, 0, 1024);
				// Check if it was for closing, and also read the incoming message
				if ((valread = read(sd, buffer, 1024)) == 0) {
					// Somebody disconnected, get his details and print
					getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
					printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					// Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				} else {
					trim(buffer);
//					buffer[valread] = '\0';
					printf("Received: %s\n", buffer);

					if (botEnabled[i]) {
						if (startsWith(buffer, "/chatbot logout") == 0) {
							botEnabled[i] = false;
							send(sd, "stupidbot> Bye! Have a nice day and do not complain about me\nuser> ",
								 strlen("stupidbot> Bye! Have a nice day and do not complain about me\nuser> "), 0);
						} else {
							char *reply = getBotReply(botMessages, buffer);
							send(sd, reply, strlen(reply), 0);
							free(reply);
						}

					} else if (botV2Enabled[i]) {
						if (startsWith(buffer, "/chatbot_v2 logout") == 0) {
							botEnabled[i] = false;
							send(sd, "gpt2bot> Bye! Have a nice day and hope you do not have any complaints about me\nuser> ",
								 strlen("gpt2bot> Bye! Have a nice day and hope you do not have any complaints about me\nuser> "), 0);
						} else {
							char *reply = getGptBotReply(buffer);
							if (reply == NULL) {
								send(sd, "gpt2bot> System Malfunction, I couldn't understand your query\nuser> ",
									 strlen("gpt2bot> System Malfunction, I couldn't understand your query\nuser> "), 0);
							} else {
								send(sd, reply, strlen(reply), 0);
								free(reply);
							};
						}
					}
					else if (startsWith(buffer, "/logout") == 0) {
						char message[1024] = "Bye!! Have a nice day\n";
						send(sd, message, strlen(message), 0);
						close(sd);
						client_socket[i] = 0;
						client_uuids[i][0] = '\0';
					} else if (startsWith(buffer, "/active") == 0) {
						sendActiveClients(client_socket, client_uuids, i);
					} else if (startsWith(buffer, "/send") == 0) {
						chatHistory = sendMessageToClient(client_socket, client_uuids, buffer, i, chatHistory);
					} else if (startsWith(buffer, "/chatbot login") == 0) {
						botEnabled[i] = true;
						send(sd, "stupidbot> Hi, I am stupid bot, I am able to answer a limited set of your\n"
								 "questions\nuser> ",
							 strlen("stupidbot> Hi, I am stupid bot, I am able to answer a limited set of your\n"
									"questions\nuser> "), 0);
					} else if (startsWith(buffer, "/history ") == 0) {
						if (chatHistory == NULL) {
							send(sd, "No chat history\nuser> ", strlen("No chat history\nuser> "), 0);
						} else {

							char **splited = split_string(buffer, " ", 2);
							if (splited[1] == NULL) {
								send(sd, "Invalid command\nuser> ", strlen("Invalid command\nuser> "), 0);
								continue;
							}
							bool found = false;


							for (int j = 0; chatHistory[j] != NULL; j++) {


								if (isReceiverSender(chatHistory[j], client_uuids[i], splited[1])) {
									found = true;
									bool fromMe = startsWith(chatHistory[j]->from, client_uuids[i]) == 0;
									char message[1024] = "\nFrom: ";
									strcat(message, fromMe ? "You" : chatHistory[j]->from);
									strcat(message, "\nTo: ");
									strcat(message, fromMe ? chatHistory[j]->to : "You");
									strcat(message, "\nMessage: ");
									strcat(message, chatHistory[j]->message);
									strcat(message, "\n");
									send(sd, message, strlen(message), 0);
								}
							}

							if (!found) {
								send(sd, "No chat history with that user\nuser> ",
									 strlen("No chat history with that user\nuser> "), 0);
							} else {
								send(sd, "\nuser> ", strlen("\nuser> "), 0);
							}


						}

					} else if (startsWith(buffer, "/history_delete") == 0) {
						char **splited = split_string(buffer, " ", 2);
						if (splited[1] == NULL) {
							send(sd, "Invalid command\nuser> ", strlen("Invalid command\nuser> "), 0);
							continue;
						}

						Message **newHistory = NULL;

						if (chatHistory != NULL) {
							for (int j = 0; chatHistory[j] != NULL; j++) {
								if (!isReceiverSender(chatHistory[j], client_uuids[i], splited[1])) {
									newHistory = addMessageToChatHistory(newHistory, chatHistory[j]->to,
																		 chatHistory[j]->from, chatHistory[j]->message);
								}
							}

							chatHistory = newHistory;

							send(sd, "Chat history deleted\nuser> ", strlen("Chat history deleted\nuser> "), 0);
						} else {
							send(sd, "No chat history to delete\nuser> ", strlen("No chat history to delete\nuser> "),
								 0);
						}
					} else if (startsWith(buffer, "/delete_all") == 0) {

						Message **newHistory = NULL;

						if (chatHistory != NULL) {
							for (int j = 0; chatHistory[j] != NULL; j++) {
								if (startsWith(chatHistory[j]->from, client_uuids[i]) != 0 &&
									startsWith(chatHistory[j]->to, client_uuids[i]) != 0) {
									newHistory = addMessageToChatHistory(newHistory, chatHistory[j]->to,
																		 chatHistory[j]->from, chatHistory[j]->message);
								}
							}

							chatHistory = newHistory;

							send(sd, "Chat history deleted\nuser> ", strlen("Chat history deleted\nuser> "), 0);
						} else {
							send(sd, "No chat history to delete\nuser> ", strlen("No chat history to delete\nuser> "),
								 0);
						}

					} else if (startsWith(buffer, "/chatbot_v2 login") == 0) {
						botV2Enabled[i] = true;
						send(sd,
							 "gpt2bot> Hi, I am updated bot, I am able to answer any question be it correct or incorrect\nuser> ",
							 strlen("gpt2bot> Hi, I am updated bot, I am able to answer any question be it correct or incorrect\nuser> "),
							 0);
					} else {
						send(sd, "Sorry, I don't understand that command\nuser> ",
							 strlen("Sorry, I don't understand that command\nuser> "), 0);
					}

					memset(buffer, 0, 1024);
				}
			}
		}
	}

	return 0;
}

char *getGptReply(const char *str) {
	char command[2048];
	snprintf(command, sizeof(command), "./venv/bin/python3 gpt-inference.py \"%s\"", str);

	FILE *fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command\n");
		exit(1);
	}


	char out[2048];
	char *reply = NULL;
	while (fgets(out, sizeof(out), fp) != NULL) {
		if (reply == NULL) {
			reply = strdup(out);
		} else {
			reply = realloc(reply, strlen(reply) + strlen(out) + 1);
			if (reply != NULL ) { strcat(reply, out); }
		}
	}

	pclose(fp);

	return strdup(out);
}

char *getGptBotReply(char buffer[1025]) {
	char *out = getGptReply(buffer);
	if (out == NULL) {
		return NULL;
	}
	char *reply = malloc(strlen(out) + strlen("gpt2bot> \nuser> ") + 1);
	strcpy(reply, "gpt2bot> ");
	strcat(reply, out);
	strcat(reply, "\nuser> ");
	return reply;
}

bool isReceiverSender(Message *ptr, char string[37], char *string1) {
	return (startsWith(ptr->from, string) == 0 && startsWith(ptr->to, string1) == 0) ||
		   (startsWith(ptr->to, string) == 0 && startsWith(ptr->from, string1) == 0);
}

char *getBotReply(BotMessage **messages, char buffer[1025]) {
	for (int i = 0; messages[i] != NULL; i++) {
		if (strncmp(messages[i]->message, buffer, strlen(messages[i]->message)) == 0) {
			char *reply = malloc(strlen(messages[i]->reply) + strlen("stupidbot> \nuser> ") + 1);
			strcpy(reply, "stupidbot> ");
			strcat(reply, messages[i]->reply);
			strcat(reply, "\nuser> ");
			return reply;
		}
	}
	return "stupidbot> System Malfunction, I couldn't understand your query\nuser> ";
}

Message **sendMessageToClient(int *usersIds, char userIds[10][UUID4_STR_BUFFER_SIZE], char *string, int i,
							  Message **chatHistory) {
	char **splited = split_string(string, " ", 3);

	//print the splitted string

	int count = 0;
	for (int j = 0; splited[j] != NULL; j++) {
		count++;
	}

	if (count < 3) {
		send(usersIds[i], "Invalid command", strlen("Invalid command"), 0);
		return chatHistory;
	}

	//check if the user exists
	int found = 0;
	int index = 0;
	for (int j = 0; j < 10; ++j) {
		if (startsWith(userIds[j], splited[1]) == 0) {
			found = 1;
			index = j;
			break;
		}
	}

	if (found == 0) {
		send(usersIds[i], "User not found or went offline\nuser> ", strlen("User not found or went offline\nuser> "),
			 0);
		return chatHistory;
	}

	//check if user is yourself
	if (strcmp(userIds[i], splited[1]) == 0) {
		send(usersIds[i], "You can't send message to yourself\nuser> ",
			 strlen("You can't send message to yourself\nuser> "), 0);
		return chatHistory;
	}


	char msg[MAX_BUFFER_SIZE];
	strcpy(msg, "\nMessage from ");
	strcat(msg, userIds[i]);
	strcat(msg, "\nMessage: ");
	strcat(msg, splited[2]);
	strcat(msg, "\nuser> ");

	int val = send(usersIds[index], msg, strlen(msg) + 1, 0);

	if (val < 0) {
		send(usersIds[i], "Failed to send message \nuser> ", strlen("Failed to send message \nuser> "), 0);
		return chatHistory;
	} else {
		chatHistory = addMessageToChatHistory(chatHistory, splited[1], userIds[i], splited[2]);
		send(usersIds[i], "Message sent \nuser> ", strlen("Message sent \nuser> "), 0);
		return chatHistory;
	}
}

Message **addMessageToChatHistory(Message **history, char *string, char string1[37], char *string2) {
	Message *msg = malloc(sizeof(Message));
	msg->from = strdup(string1);
	msg->to = strdup(string);
	msg->message = strdup(string2);


	if (history == NULL) {
		history = malloc(sizeof(Message *) * 2);
		history[0] = msg;
		history[1] = NULL;
		return history;
	}
	int currentSize = 0;
	for (int i = 0; history[i] != NULL; i++) {
		currentSize++;
	}

	history = realloc(history, sizeof(Message *) * (currentSize + 2));
	history[currentSize] = msg;
	history[currentSize + 1] = NULL;

	return history;

}


void sendActiveClients(int *pInt, char pString[10][UUID4_STR_BUFFER_SIZE], int i) {
	char message[1024] = "Active clients: ";
	for (int j = 0; j < 10; ++j) {
		if (pInt[j] != 0) {
			printf("Found active client %d\n", j);
			printf("Client UUID: %s\n", pString[j]);
			strcat(message, pString[j]);
			strcat(message, "\n");
		}
	}
	printf("%s\n", message);
	strcat(message, "\nuser> ");
	send(pInt[i], message, strlen(message), 0);

}

