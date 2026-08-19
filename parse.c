/**
 * Reading of the arguments of the commands given in the standard input.
 * @file parse.c
 * @author ist1110573
 */

#include <stdio.h>
#include <stdlib.h>
#include "project.h"

/** Number of fields of a date and of an hour. */
#define DATE_FIELDS 3
#define TIME_FIELDS 2

/**
 * Allocates memory and aborts the program when there is none available.
 * @param size number of bytes to allocate
 * @return Pointer to the allocated memory.
 */
void *safe_alloc(size_t size) {
	void *memory = calloc(1, size);
	if (memory == NULL) {
		exit(EXIT_FAILURE);
	}
	return memory;
}

/**
 * Tells whether a character separates two arguments of a command.
 * @param c character to classify
 * @return 1 if the character is blank, 0 otherwise.
 */
static int is_blank(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/**
 * Reads the next argument of a command. Arguments delimited by quotes may
 * contain blank characters, the quotes are not copied to the token.
 * @param line text still to be read
 * @param token buffer that receives the argument
 * @return Pointer to the text after the argument, NULL if there is none.
 */
char *read_token(char *line, char *token) {
	int i = 0;
	char end = ' ';
	while (is_blank(*line)) {
		line++;
	}
	if (*line == '\0') {
		return NULL;
	}
	if (*line == '"') {
		end = *line++;
	}
	while (*line != '\0' && *line != end && !(end == ' ' && is_blank(*line))) {
		token[i++] = *line++;
	}
	token[i] = '\0';
	if (*line == '"') {
		line++;
	}
	return line;
}

/**
 * Reads a date, in the format DD-MM-AAAA, from the next argument.
 * @param line text still to be read
 * @param date structure that receives the date
 * @return Pointer to the text after the date, NULL if there is none.
 */
char *read_date(char *line, Date *date) {
	char token[MAX_LINE];
	line = read_token(line, token);
	if (line == NULL) {
		return NULL;
	}
	if (sscanf(token, "%d-%d-%d", &date->day, &date->month,
			&date->year) != DATE_FIELDS) {
		return NULL;
	}
	return line;
}

/**
 * Reads a date and an hour, in the format DD-MM-AAAA HH:MM.
 * @param line text still to be read
 * @param date structure that receives the date
 * @param time structure that receives the hour
 * @return Pointer to the text after the hour, NULL if it is missing.
 */
char *read_date_time(char *line, Date *date, Time *time) {
	char token[MAX_LINE];
	line = read_date(line, date);
	if (line == NULL) {
		return NULL;
	}
	line = read_token(line, token);
	if (line == NULL) {
		return NULL;
	}
	if (sscanf(token, "%d:%d", &time->hour,
			&time->minutes) != TIME_FIELDS) {
		return NULL;
	}
	return line;
}
