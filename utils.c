// #define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

int startsWith(const char *buffer, const char *string) {
	int i = 0;
	while (string[i] != '\0') {
		if (buffer[i] != string[i]) {
			return 1;
		}
		i++;
	}
	return 0;
}

char **split_string(char *str, char *delim, int limit) {
	int i, j, k, m, numwords = 0;
	char **s;

	if (str == NULL || str[0] == 0)
		return (NULL);
	if (!delim)
		delim = " ";

	numwords = count_words(str, delim, numwords);
	if (numwords == 0) {
		return (NULL);
	}
	s = malloc((1 + numwords) * sizeof(char *));
	if (!s)
		return (NULL);
	for (i = 0, j = 0; j < numwords; j++) {
		if (limit == j) {
			break;
		}

		while (is_delim(str[i], delim) == 1)
			i++;


		k = 0;
		while (!is_delim(str[i + k], delim) && str[i + k]) {
			k++;
		}

		if(j == limit - 1) {
			while(str[i + k]){
				k++;
			}
		}

		s[j] = malloc((k + 1) * sizeof(char));
		if (!s[j]) {
			for (k = 0; k < j; k++)
				free(s[k]);

			free(s);
			return (NULL);
		}
		for (m = 0; m < k; m++)
			s[j][m] = str[i++];
		s[j][m] = 0;
	}
	s[j] = NULL;
	return (s);
}

/**
 * is_delim - checks if character is a delimiter
 * @i: character to check
 * @delim: delimiter to check against
 * Return: 1 if true, 0 if false
 */
int is_delim(char i, const char *delim) {
	int j = 0;

	while (delim[j] != '\0') {
		if (i == delim[j])
			return (1);
		j++;
	}
	return (0);
}

/**
 * count_words - count words in a string
 * @str: string to count words in
 * @delim: delimiter to split words by
 * @numwords: number of words
 * Return: number of words
 */
int count_words(const char *str, const char *delim, int numwords) {
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (!is_delim(str[i], delim) &&
			(is_delim(str[i + 1], delim) || !str[i + 1])) {
			numwords++;
		}
	}
	return (numwords);
}

/**
 * isspace_ - Checks if a character is a space.
 * @c: The character.
 * Return: 1 if the character is a space, 0 otherwise.
 */
int isspace_(int c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
		c == '\r')
	{
		return (1);
	}
	return (0);
}


/**
 * trim - Trims a string.
 * @str: The string.
 * Return: Nothing.
 * Description: Trims a string.
 */
void trim(char *str)
{
	int i;
	int start = 0;
	int end = (int) strlen_(str) - 1;

	while (isspace_(str[start]))
		start++;

	while (end >= start && (isspace_(str[end]) || str[end] == '\n'))
		end--;

	for (i = start; i <= end; i++)
		str[i - start] = str[i];

	str[end - start + 1] = '\0';
}

BotMessage **loadBotMessages() {
	FILE *file = fopen("FAQs.txt", "r");
	if (file == NULL) {
		printf("Error opening file\n");
		exit(1);
	}

	BotMessage **messages = NULL;
	size_t messagesAllocated = 0;
	size_t messageCount = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, file)) != -1) {
		char **parts = split_string(line, "|||", 2);

		if (messageCount >= messagesAllocated) {
			size_t newCapacity = messagesAllocated == 0 ? 1 : messagesAllocated * 2;
			BotMessage **newMessages = realloc(messages, newCapacity * sizeof(BotMessage *));
			if (!newMessages) {
				perror("Failed to reallocate memory");
				// Cleanup code here (free previously allocated memory)
				exit(1);
			}
			messages = newMessages;
			messagesAllocated = newCapacity;
		}

		BotMessage *newMessage = malloc(sizeof(BotMessage));
		if (!newMessage) {
			perror("Failed to allocate memory for new message");
			// Cleanup code here
			exit(1);
		}

		trim(parts[0]);
		trim(parts[1]);
		newMessage->message = strdup(parts[0]);
		newMessage->reply = strdup(parts[1]);

		messages[messageCount++] = newMessage;

		free(parts); // Assuming split_string allocates memory for parts array itself but not for strings inside
	}

	fclose(file);
	free(line);

	// Null-terminate the array of BotMessage pointers
	if (messageCount < messagesAllocated) {
		messages[messageCount] = NULL; // Assuming caller knows to stop at NULL
	}

	return messages;
}

/**
 * strlen_ - Gets the length of a string.
 * @str: The string.
 * Return: The length of the string.
 */
int strlen_(const char *str)
{
	int i = 0;

	while (str[i] != '\0')
		i++;
	return (i);
}

char* readDataFromSocket(int sock) {
	ssize_t valread;
	char readBuffer[READ_BUFFER_SIZE];
	char *dataBuffer = NULL;
	size_t dataBufferSize = 0;

	while ((valread = read(sock, readBuffer, READ_BUFFER_SIZE)) > 0) {
		// Resize the dynamic buffer to accommodate new data
		char *newBuffer = realloc(dataBuffer, dataBufferSize + valread);
		if (!newBuffer) {
			perror("Failed to reallocate memory");
			free(dataBuffer); // Free any allocated memory before exiting
			return NULL;
		}
		dataBuffer = newBuffer;
		// Copy new data to the end of the dynamic buffer
		memcpy(dataBuffer + dataBufferSize, readBuffer, valread);
		dataBufferSize += valread;
	}

	if (valread < 0) {
		// Handle read error
		perror("read");
		free(dataBuffer); //free allocated memory on error
		return NULL;
	}

	//  null-terminate the received data
	char *finalBuffer = realloc(dataBuffer, dataBufferSize + 1);
	if (!finalBuffer) {
		perror("Failed to reallocate memory for null terminator");
		free(dataBuffer);
		return NULL;
	}
	dataBuffer = finalBuffer;
	dataBuffer[dataBufferSize] = '\0';

	return dataBuffer; // Caller is responsible for freeing this memory
}
