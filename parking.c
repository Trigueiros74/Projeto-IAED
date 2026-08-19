/**
 * Creation, listing and removal of parking lots (commands 'p' and 'r').
 * @file parking.c
 * @author ist1110573
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"

/** Number of billing values of the 'p' command. */
#define COST_FIELDS 4

/**
 * Searches for a parking lot by name.
 * @param sys state of the system
 * @param name name of the parking lot
 * @return The index of the parking lot, or -1 if it does not exist.
 */
int find_park(System *sys, char *name) {
	int i;
	for (i = 0; i < sys->num_parks; i++) {
		if (strcmp(sys->parks[i]->name, name) == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * Copies the parking lots of the system into a vector sorted by name and
 * records in each parking lot its position in that order.
 * @param sys state of the system
 * @param sorted vector with room for all the parking lots
 */
void sort_parks_by_name(System *sys, Park *sorted[]) {
	int i, j;
	for (i = 0; i < sys->num_parks; i++) {
		sorted[i] = sys->parks[i];
	}
	for (i = 1; i < sys->num_parks; i++) {
		Park *park = sorted[i];
		for (j = i; j > 0 &&
				strcmp(sorted[j - 1]->name, park->name) > 0; j--) {
			sorted[j] = sorted[j - 1];
		}
		sorted[j] = park;
	}
	for (i = 0; i < sys->num_parks; i++) {
		sorted[i]->order = i;
	}
}

/**
 * Writes the name, the capacity and the free spaces of every parking lot,
 * by order of creation.
 */
static void list_parks(System *sys) {
	int i;
	for (i = 0; i < sys->num_parks; i++) {
		printf("%s %d %d\n", sys->parks[i]->name,
			sys->parks[i]->capacity, sys->parks[i]->available);
	}
}

/**
 * Verifies the arguments of a new parking lot and writes the first error
 * found, by the order defined in the statement.
 * @return 1 if the parking lot may be created, 0 otherwise.
 */
static int valid_new_park(System *sys, char *name, int capacity, Cost cost) {
	if (find_park(sys, name) >= 0) {
		printf("%s: %s", name, PARKING_ALREADY_EXISTS);
		return 0;
	}
	if (capacity <= 0) {
		printf("%d: %s", capacity, INVALID_CAPACITY);
		return 0;
	}
	if (cost.first_hour <= 0 || cost.after_hour <= cost.first_hour ||
			cost.daily <= cost.after_hour) {
		printf("%s", INVALID_COST);
		return 0;
	}
	if (sys->num_parks == MAX_PARKS) {
		printf("%s", TOO_MANY_PARKS);
		return 0;
	}
	return 1;
}

/**
 * Creates a parking lot with no vehicles inside.
 * @return Pointer to the new parking lot.
 */
static Park *new_park(char *name, int capacity, Cost cost) {
	Park *park = safe_alloc(sizeof(Park));
	park->name = safe_alloc(strlen(name) + 1);
	strcpy(park->name, name);
	park->capacity = capacity;
	park->available = capacity;
	park->cost = cost;
	return park;
}

/**
 * Executes the command 'p': lists the parking lots when it has no
 * arguments, otherwise creates a new parking lot.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_park(System *sys, char *args) {
	char name[MAX_LINE];
	int capacity;
	Cost cost;
	char *rest = read_token(args, name);
	if (rest == NULL) {
		list_parks(sys);
		return;
	}
	if (sscanf(rest, "%d %lf %lf %lf", &capacity, &cost.first_hour,
			&cost.after_hour, &cost.daily) != COST_FIELDS) {
		return;
	}
	if (valid_new_park(sys, name, capacity, cost)) {
		sys->parks[sys->num_parks++] = new_park(name, capacity, cost);
	}
}

/**
 * Removes every stay of a parking lot, detaching them from the vehicles.
 */
static void free_history(Park *park) {
	Registry *reg = park->first, *next;
	while (reg != NULL) {
		next = reg->next_in_park;
		detach_registry(reg);
		free(reg);
		reg = next;
	}
}

/**
 * Releases all the memory used by a parking lot.
 */
void free_park(Park *park) {
	free_history(park);
	free_billing(park);
	free(park->name);
	free(park);
}

/**
 * Executes the command 'r': removes a parking lot and all its stays, and
 * writes the names of the remaining parking lots sorted by name.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_remove_park(System *sys, char *args) {
	char name[MAX_LINE];
	Park *sorted[MAX_PARKS];
	int park, i;
	if (read_token(args, name) == NULL) {
		return;
	}
	park = find_park(sys, name);
	if (park < 0) {
		printf("%s: %s", name, NO_SUCH_PARKING);
		return;
	}
	free_park(sys->parks[park]);
	for (i = park; i < sys->num_parks - 1; i++) {
		sys->parks[i] = sys->parks[i + 1];
	}
	sys->num_parks--;
	sort_parks_by_name(sys, sorted);
	for (i = 0; i < sys->num_parks; i++) {
		printf("%s\n", sorted[i]->name);
	}
}
