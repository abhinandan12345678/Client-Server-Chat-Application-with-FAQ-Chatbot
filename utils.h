//
// Created by felixmarvin on 3/28/24.
//

#ifndef FAQ_D1_UTILS_H
#define FAQ_D1_UTILS_H
#define READ_BUFFER_SIZE 1024
#define MAX_BUFFER_SIZE 5000
typedef struct {
	char *message;
	char *reply;
} BotMessage;

int startsWith(const char *buffer, const char *string);
void trim(char *str);
char **split_string(char *str, char *delim, int limit);
int strlen_(const char *str);
int count_words(const char *str, const char *delim, int numwords);
int is_delim(char i, const char *delim);
BotMessage **loadBotMessages();
char* readDataFromSocket(int sock);
int isspace_(int c);

#endif //FAQ_D1_UTILS_H
