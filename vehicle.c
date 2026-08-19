/**
 * Entries, exits and history of the vehicles (commands 'e', 's' and 'v').
 * @file vehicle.c
 * @author ist1110573
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "project.h"

/**
 * Verifies that a licence plate is a sequence of three groups, separated by
 * '-', where each group has two capital letters or two decimal digits, with
 * at least one group of each kind.
 * @param plate licence plate to validate
 * @return 1 if the licence plate is valid, 0 otherwise.
 */
int valid_plate(char *plate) {
	int i, letters = 0, digits = 0;
	if (strlen(plate) != PLATE_LENGTH) {
		return 0;
	}
	for (i = PLATE_GROUP; i < PLATE_LENGTH; i += PLATE_GROUP + 1) {
		if (plate[i] != '-') {
			return 0;
		}
	}
	for (i = 0; i < PLATE_LENGTH; i += PLATE_GROUP + 1) {
		if (isupper((unsigned char) plate[i]) &&
				isupper((unsigned char) plate[i + 1])) {
			letters++;
		} else if (isdigit((unsigned char) plate[i]) &&
				isdigit((unsigned char) plate[i + 1])) {
			digits++;
		} else {
			return 0;
		}
	}
	return letters > 0 && digits > 0;
}

/**
 * Computes the hash table bucket of a licence plate.
 */
static unsigned long hash_plate(char *plate) {
	unsigned long hash = 0;
	while (*plate != '\0') {
		hash = hash * HASH_FACTOR + (unsigned char) *plate++;
	}
	return hash % HASH_SIZE;
}

/**
 * Searches the hash table for the vehicle with a given licence plate.
 * @return Pointer to the vehicle, or NULL if it was never registered.
 */
Vehicle *find_vehicle(System *sys, char *plate) {
	Vehicle *vehicle = sys->vehicles[hash_plate(plate)];
	while (vehicle != NULL && strcmp(vehicle->plate, plate) != 0) {
		vehicle = vehicle->next;
	}
	return vehicle;
}

/**
 * Inserts a new vehicle in the hash table.
 * @return Pointer to the new vehicle.
 */
static Vehicle *add_vehicle(System *sys, char *plate) {
	unsigned long bucket = hash_plate(plate);
	Vehicle *vehicle = safe_alloc(sizeof(Vehicle));
	strcpy(vehicle->plate, plate);
	vehicle->next = sys->vehicles[bucket];
	sys->vehicles[bucket] = vehicle;
	return vehicle;
}

/**
 * Removes a stay from the history of its vehicle, so that the stay may be
 * released when the parking lot where it happened is removed.
 */
void detach_registry(Registry *reg) {
	Vehicle *vehicle = reg->vehicle;
	if (reg->prev_in_vehicle == NULL) {
		vehicle->first = reg->next_in_vehicle;
	} else {
		reg->prev_in_vehicle->next_in_vehicle = reg->next_in_vehicle;
	}
	if (reg->next_in_vehicle == NULL) {
		vehicle->last = reg->prev_in_vehicle;
	} else {
		reg->next_in_vehicle->prev_in_vehicle = reg->prev_in_vehicle;
	}
	if (vehicle->inside == reg) {
		vehicle->inside = NULL;
	}
}

/**
 * Verifies the errors of an entry and writes the first one found, by the
 * order defined in the statement.
 * @return 1 if the entry may be registered, 0 otherwise.
 */
static int valid_entry(System *sys, int park, char *name, char *plate,
			Date date, Time time) {
	Vehicle *vehicle;
	if (park < 0) {
		printf("%s: %s", name, NO_SUCH_PARKING);
		return 0;
	}
	if (sys->parks[park]->available == 0) {
		printf("%s: %s", name, PARKING_FULL);
		return 0;
	}
	if (!valid_plate(plate)) {
		printf("%s: %s", plate, INVALID_PLATE);
		return 0;
	}
	vehicle = find_vehicle(sys, plate);
	if (vehicle != NULL && vehicle->inside != NULL) {
		printf("%s: %s", plate, INVALID_ENTRY);
		return 0;
	}
	if (!valid_event_time(sys, date, time)) {
		printf("%s", INVALID_DATE);
		return 0;
	}
	return 1;
}

/**
 * Registers the entry of a vehicle, creating a new stay and linking it to
 * the history of the parking lot and to the history of the vehicle.
 */
static void register_entry(System *sys, Park *park, char *plate,
			Date date, Time time) {
	Vehicle *vehicle = find_vehicle(sys, plate);
	Registry *reg = safe_alloc(sizeof(Registry));
	if (vehicle == NULL) {
		vehicle = add_vehicle(sys, plate);
	}
	reg->park = park;
	reg->vehicle = vehicle;
	reg->entry_date = date;
	reg->entry_time = time;
	if (park->last == NULL) {
		park->first = reg;
	} else {
		park->last->next_in_park = reg;
	}
	park->last = reg;
	reg->prev_in_vehicle = vehicle->last;
	if (vehicle->last == NULL) {
		vehicle->first = reg;
	} else {
		vehicle->last->next_in_vehicle = reg;
	}
	vehicle->last = reg;
	vehicle->inside = reg;
	park->available--;
	set_last_event(sys, date, time);
}

/**
 * Executes the command 'e': registers the entry of a vehicle and writes the
 * spaces left in the parking lot.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_entry(System *sys, char *args) {
	char name[MAX_LINE], plate[MAX_LINE];
	Date date;
	Time time;
	int park;
	char *rest = read_token(args, name);
	if (rest == NULL || (rest = read_token(rest, plate)) == NULL) {
		return;
	}
	if (read_date_time(rest, &date, &time) == NULL) {
		return;
	}
	park = find_park(sys, name);
	if (!valid_entry(sys, park, name, plate, date, time)) {
		return;
	}
	register_entry(sys, sys->parks[park], plate, date, time);
	printf("%s %d\n", name, sys->parks[park]->available);
}

/**
 * Verifies the errors of an exit and writes the first one found, by the
 * order defined in the statement.
 * @return 1 if the exit may be registered, 0 otherwise.
 */
static int valid_exit(System *sys, int park, char *name, char *plate,
			Date date, Time time) {
	Vehicle *vehicle;
	if (park < 0) {
		printf("%s: %s", name, NO_SUCH_PARKING);
		return 0;
	}
	if (!valid_plate(plate)) {
		printf("%s: %s", plate, INVALID_PLATE);
		return 0;
	}
	vehicle = find_vehicle(sys, plate);
	if (vehicle == NULL || vehicle->inside == NULL ||
			vehicle->inside->park != sys->parks[park]) {
		printf("%s: %s", plate, INVALID_EXIT);
		return 0;
	}
	if (!valid_event_time(sys, date, time)) {
		printf("%s", INVALID_DATE);
		return 0;
	}
	return 1;
}

/**
 * Closes the stay of a vehicle, charges it and frees its space.
 * @return Pointer to the closed stay.
 */
static Registry *register_exit(System *sys, Park *park, Vehicle *vehicle,
			Date date, Time time) {
	Registry *reg = vehicle->inside;
	reg->exit_date = date;
	reg->exit_time = time;
	reg->closed = 1;
	reg->fee = parking_fee(park, reg);
	vehicle->inside = NULL;
	park->available++;
	bill_registry(park, reg);
	set_last_event(sys, date, time);
	return reg;
}

/**
 * Executes the command 's': registers the exit of a vehicle and writes the
 * period of the stay and the value charged.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_exit(System *sys, char *args) {
	char name[MAX_LINE], plate[MAX_LINE];
	Date date;
	Time time;
	int park;
	Registry *reg;
	char *rest = read_token(args, name);
	if (rest == NULL || (rest = read_token(rest, plate)) == NULL) {
		return;
	}
	if (read_date_time(rest, &date, &time) == NULL) {
		return;
	}
	park = find_park(sys, name);
	if (!valid_exit(sys, park, name, plate, date, time)) {
		return;
	}
	reg = register_exit(sys, sys->parks[park],
			find_vehicle(sys, plate), date, time);
	printf("%s %02d-%02d-%d %02d:%02d %02d-%02d-%d %02d:%02d %.2f\n",
		plate, reg->entry_date.day, reg->entry_date.month,
		reg->entry_date.year, reg->entry_time.hour,
		reg->entry_time.minutes, reg->exit_date.day,
		reg->exit_date.month, reg->exit_date.year,
		reg->exit_time.hour, reg->exit_time.minutes, reg->fee);
}

/**
 * Writes a stay: the parking lot, the entry and, if the vehicle has already
 * left, the exit.
 */
static void print_registry(Registry *reg) {
	printf("%s %02d-%02d-%d %02d:%02d", reg->park->name,
		reg->entry_date.day, reg->entry_date.month,
		reg->entry_date.year, reg->entry_time.hour,
		reg->entry_time.minutes);
	if (reg->closed) {
		printf(" %02d-%02d-%d %02d:%02d", reg->exit_date.day,
			reg->exit_date.month, reg->exit_date.year,
			reg->exit_time.hour, reg->exit_time.minutes);
	}
	printf("\n");
}

/**
 * Writes the stays of a vehicle sorted by the name of the parking lot and,
 * inside each parking lot, by the entry instant. The stays are distributed
 * by parking lot with a counting sort; as the history of the vehicle is
 * already chronological, the entries of each parking lot keep their order.
 */
static void print_vehicle_history(System *sys, Vehicle *vehicle) {
	Park *sorted[MAX_PARKS];
	Registry *reg, **stays;
	int start[MAX_PARKS + 1] = {0};
	int i, total = 0;
	sort_parks_by_name(sys, sorted);
	for (reg = vehicle->first; reg != NULL; reg = reg->next_in_vehicle) {
		start[reg->park->order + 1]++;
		total++;
	}
	for (i = 0; i < sys->num_parks; i++) {
		start[i + 1] += start[i];
	}
	stays = safe_alloc((size_t) total * sizeof(Registry *));
	for (reg = vehicle->first; reg != NULL; reg = reg->next_in_vehicle) {
		stays[start[reg->park->order]++] = reg;
	}
	for (i = 0; i < total; i++) {
		print_registry(stays[i]);
	}
	free(stays);
}

/**
 * Executes the command 'v': writes all the stays of a vehicle.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_list_vehicle(System *sys, char *args) {
	char plate[MAX_LINE];
	Vehicle *vehicle;
	if (read_token(args, plate) == NULL) {
		return;
	}
	if (!valid_plate(plate)) {
		printf("%s: %s", plate, INVALID_PLATE);
		return;
	}
	vehicle = find_vehicle(sys, plate);
	if (vehicle == NULL || vehicle->first == NULL) {
		printf("%s: %s", plate, NO_ENTRIES_IN_PARKING);
		return;
	}
	print_vehicle_history(sys, vehicle);
}

/**
 * Releases the hash table of the vehicles. The stays are released with the
 * parking lots where they happened.
 */
void free_vehicles(System *sys) {
	Vehicle *vehicle, *next;
	int i;
	for (i = 0; i < HASH_SIZE; i++) {
		vehicle = sys->vehicles[i];
		while (vehicle != NULL) {
			next = vehicle->next;
			free(vehicle);
			vehicle = next;
		}
		sys->vehicles[i] = NULL;
	}
}
