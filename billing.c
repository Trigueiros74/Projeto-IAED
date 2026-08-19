/**
 * Computation of the values charged and billing of a parking lot
 * (command 'f').
 * @file billing.c
 * @author ist1110573
 */

#include <stdio.h>
#include <stdlib.h>
#include "project.h"

/**
 * Computes the value charged for a stay. Every complete period of 24 hours
 * is charged at the daily rate; the remaining time is charged by periods of
 * 15 minutes, the first four at the rate of the first hour and the others
 * at the rate after the first hour, never exceeding the daily rate.
 * @param park parking lot where the vehicle stayed
 * @param reg stay already closed
 * @return The value to charge.
 */
double parking_fee(Park *park, Registry *reg) {
	long total = date_to_minutes(reg->exit_date, reg->exit_time)
		- date_to_minutes(reg->entry_date, reg->entry_time);
	long periods = (total % MINUTES_PER_DAY + PERIOD - 1) / PERIOD;
	double fee = (total / MINUTES_PER_DAY) * park->cost.daily;
	double day_fee;
	if (periods <= FIRST_HOUR_PERIODS) {
		day_fee = periods * park->cost.first_hour;
	} else {
		day_fee = FIRST_HOUR_PERIODS * park->cost.first_hour
			+ (periods - FIRST_HOUR_PERIODS) * park->cost.after_hour;
	}
	if (day_fee > park->cost.daily) {
		day_fee = park->cost.daily;
	}
	return fee + day_fee;
}

/**
 * Adds a closed stay to the billing of the day of its exit. As the exits
 * are chronological, the days are created by increasing order.
 * @param park parking lot that charged the stay
 * @param reg stay already closed
 */
void bill_registry(Park *park, Registry *reg) {
	DailyBill *day = park->last_bill;
	if (day == NULL || compare_dates(day->date, reg->exit_date) != 0) {
		day = safe_alloc(sizeof(DailyBill));
		day->date = reg->exit_date;
		if (park->last_bill == NULL) {
			park->first_bill = day;
		} else {
			park->last_bill->next = day;
		}
		park->last_bill = day;
	}
	if (day->last == NULL) {
		day->first = reg;
	} else {
		day->last->next_in_day = reg;
	}
	day->last = reg;
	day->total += reg->fee;
}

/**
 * Releases the billing days of a parking lot.
 */
void free_billing(Park *park) {
	DailyBill *day = park->first_bill, *next;
	while (day != NULL) {
		next = day->next;
		free(day);
		day = next;
	}
	park->first_bill = NULL;
	park->last_bill = NULL;
}

/**
 * Writes the total billed by a parking lot on each day, sorted by date.
 */
static void print_daily_billing(Park *park) {
	DailyBill *day;
	for (day = park->first_bill; day != NULL; day = day->next) {
		printf("%02d-%02d-%d %.2f\n", day->date.day, day->date.month,
			day->date.year, day->total);
	}
}

/**
 * Writes the licence plate, the exit hour and the value charged to every
 * vehicle billed by a parking lot on a given day, sorted by exit hour.
 */
static void print_day_billing(Park *park, Date date) {
	DailyBill *day = park->first_bill;
	Registry *reg;
	while (day != NULL && compare_dates(day->date, date) != 0) {
		day = day->next;
	}
	if (day == NULL) {
		return;
	}
	for (reg = day->first; reg != NULL; reg = reg->next_in_day) {
		printf("%s %02d:%02d %.2f\n", reg->vehicle->plate,
			reg->exit_time.hour, reg->exit_time.minutes, reg->fee);
	}
}

/**
 * Verifies that a date may be consulted, that is, that it exists and that
 * it is not later than the day of the last entry or exit of the system.
 * @return 1 if the date may be consulted, 0 otherwise.
 */
static int valid_billing_date(System *sys, Date date) {
	Time midnight = {0, 0};
	if (!valid_date_time(date, midnight)) {
		return 0;
	}
	if (!sys->has_events) {
		return 1;
	}
	return compare_dates(date, sys->last_date) <= 0;
}

/**
 * Executes the command 'f': writes the daily billing of a parking lot or,
 * when a date is given, the values billed on that day.
 * @param sys state of the system
 * @param args arguments of the command
 */
void command_billing(System *sys, char *args) {
	char name[MAX_LINE];
	Date date;
	int park;
	char *rest = read_token(args, name);
	if (rest == NULL) {
		return;
	}
	park = find_park(sys, name);
	if (park < 0) {
		printf("%s: %s", name, NO_SUCH_PARKING);
		return;
	}
	if (read_date(rest, &date) == NULL) {
		print_daily_billing(sys->parks[park]);
	} else if (!valid_billing_date(sys, date)) {
		printf("%s", INVALID_DATE);
	} else {
		print_day_billing(sys->parks[park], date);
	}
}
