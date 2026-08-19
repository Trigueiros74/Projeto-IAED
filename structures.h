/**
 * Data types and constants of the parking lot management system.
 * @file structures.h
 * @author ist1110573
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

/** Maximum number of parking lots managed by the system. */
#define MAX_PARKS 20
/** Maximum size of an input line (BUFSIZ on most systems). */
#define MAX_LINE 8192
/** Number of characters of a licence plate ("XX-XX-XX"). */
#define PLATE_LENGTH 8
/** Characters of each group of a licence plate. */
#define PLATE_GROUP 2
/** Number of buckets of the vehicle hash table. */
#define HASH_SIZE 10007
/** Multiplier used by the hash function of the licence plates. */
#define HASH_FACTOR 31

/** Number of months of a year. */
#define MONTHS 12
/** Hours of a day. */
#define HOURS_PER_DAY 24
/** Minutes of an hour. */
#define MINUTES_PER_HOUR 60
/** Minutes of a day. */
#define MINUTES_PER_DAY (HOURS_PER_DAY * MINUTES_PER_HOUR)
/** Days of a year: February is closed on the 29th, so it always has 28. */
#define DAYS_PER_YEAR 365
/** Duration, in minutes, of a billing period. */
#define PERIOD 15
/** Number of periods charged at the first hour rate. */
#define FIRST_HOUR_PERIODS 4

/** Error messages written to the standard output. */
#define PARKING_ALREADY_EXISTS "parking already exists.\n"
#define INVALID_CAPACITY "invalid capacity.\n"
#define INVALID_COST "invalid cost.\n"
#define TOO_MANY_PARKS "too many parks.\n"
#define NO_SUCH_PARKING "no such parking.\n"
#define PARKING_FULL "parking is full.\n"
#define INVALID_PLATE "invalid licence plate.\n"
#define INVALID_ENTRY "invalid vehicle entry.\n"
#define INVALID_DATE "invalid date.\n"
#define INVALID_EXIT "invalid vehicle exit.\n"
#define NO_ENTRIES_IN_PARKING "no entries found in any parking.\n"

/** A calendar date. */
typedef struct {
	int day;
	int month;
	int year;
} Date;

/** An hour of the day. */
typedef struct {
	int hour;
	int minutes;
} Time;

/** The billing rates of a parking lot. */
typedef struct {
	double first_hour;	/* value of each period of the first hour */
	double after_hour;	/* value of each period after the first hour */
	double daily;		/* maximum value charged per 24 hours */
} Cost;

/**
 * A stay of a vehicle in a parking lot. It is created on the entry of the
 * vehicle and completed when the vehicle leaves the parking lot.
 */
typedef struct Registry {
	struct Park *park;
	struct Vehicle *vehicle;
	Date entry_date;
	Time entry_time;
	Date exit_date;
	Time exit_time;
	int closed;		/* true after the vehicle has left */
	double fee;		/* value charged on the exit */
	struct Registry *next_in_park;
	struct Registry *next_in_day;
	struct Registry *next_in_vehicle;
	struct Registry *prev_in_vehicle;
} Registry;

/** The stays billed by a parking lot on a single day. */
typedef struct DailyBill {
	Date date;
	double total;
	Registry *first;	/* stays billed on this day, in exit order */
	Registry *last;
	struct DailyBill *next;
} DailyBill;

/** A parking lot and the history of its usage. */
typedef struct Park {
	char *name;
	int capacity;
	int available;
	int order;		/* position in the alphabetical order */
	Cost cost;
	Registry *first;	/* stays of the park, in chronological order */
	Registry *last;
	DailyBill *first_bill;	/* daily billing, in chronological order */
	DailyBill *last_bill;
} Park;

/** A vehicle, identified by its licence plate. */
typedef struct Vehicle {
	char plate[PLATE_LENGTH + 1];
	Registry *first;	/* stays of the vehicle, in chronological order */
	Registry *last;
	Registry *inside;	/* current stay, NULL if not parked */
	struct Vehicle *next;	/* next vehicle of the same hash bucket */
} Vehicle;

/** The whole state of the system. */
typedef struct {
	Park *parks[MAX_PARKS];
	int num_parks;
	Vehicle *vehicles[HASH_SIZE];	/* hash table indexed by plate */
	Date last_date;			/* date of the last entry or exit */
	Time last_time;			/* hour of the last entry or exit */
	int has_events;			/* true after the first entry */
} System;

#endif
