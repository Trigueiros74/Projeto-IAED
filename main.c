/**
 * Management system of parking lots: reads the commands from the standard
 * input and writes the answers to the standard output.
 * @file main.c
 * @author ist1110573
 */

#include <stdio.h>
#include <stdlib.h>
#include "project.h"

/**
 * Executes one command line.
 * @param sys state of the system
 * @param line line read from the standard input
 */
static void run_command(System *sys, char *line) {
	switch (line[0]) {
	case 'p':
		command_park(sys, line + 1);
		break;
	case 'e':
		command_entry(sys, line + 1);
		break;
	case 's':
		command_exit(sys, line + 1);
		break;
	case 'v':
		command_list_vehicle(sys, line + 1);
		break;
	case 'f':
		command_billing(sys, line + 1);
		break;
	case 'r':
		command_remove_park(sys, line + 1);
		break;
	default:
		break;
	}
}

/**
 * Releases all the memory used by the system.
 */
static void free_system(System *sys) {
	int i;
	for (i = 0; i < sys->num_parks; i++) {
		free_park(sys->parks[i]);
	}
	sys->num_parks = 0;
	free_vehicles(sys);
	free(sys);
}

/**
 * Reads and executes commands until the command 'q' or the end of the
 * input.
 * @return Always returns 0.
 */
int main(void) {
	char line[MAX_LINE];
	System *sys = safe_alloc(sizeof(System));
	while (fgets(line, sizeof(line), stdin) != NULL && line[0] != 'q') {
		run_command(sys, line);
	}
	free_system(sys);
	return 0;
}
