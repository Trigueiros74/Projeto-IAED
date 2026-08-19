/**
 * Validation and comparison of the dates and hours of the system.
 * @file dates.c
 * @author ist1110573
 */

#include "project.h"

/**
 * Number of days of each month. The parking lots are always closed on the
 * 29th of February, so that day is never valid nor charged.
 */
static const int days_in_month[MONTHS] = {31, 28, 31, 30, 31, 30,
					31, 31, 30, 31, 30, 31};

/**
 * Verifies that a date and an hour exist in the calendar.
 * @param date date to validate
 * @param time hour to validate
 * @return 1 if the instant is valid, 0 otherwise.
 */
int valid_date_time(Date date, Time time) {
	if (date.year < 0 || date.month < 1 || date.month > MONTHS) {
		return 0;
	}
	if (date.day < 1 || date.day > days_in_month[date.month - 1]) {
		return 0;
	}
	if (time.hour < 0 || time.hour >= HOURS_PER_DAY) {
		return 0;
	}
	return time.minutes >= 0 && time.minutes < MINUTES_PER_HOUR;
}

/**
 * Converts an instant into the number of minutes since the year 0, which
 * allows instants to be compared and subtracted.
 * @param date date to convert
 * @param time hour to convert
 * @return The instant measured in minutes.
 */
long date_to_minutes(Date date, Time time) {
	long days = (long) date.year * DAYS_PER_YEAR + date.day - 1;
	int month;
	for (month = 1; month < date.month; month++) {
		days += days_in_month[month - 1];
	}
	return days * MINUTES_PER_DAY + time.hour * MINUTES_PER_HOUR
		+ time.minutes;
}

/**
 * Compares two instants.
 * @return -1 if the first is earlier, 1 if it is later, 0 if they are equal.
 */
int compare_date_time(Date date1, Time time1, Date date2, Time time2) {
	long first = date_to_minutes(date1, time1);
	long second = date_to_minutes(date2, time2);
	if (first < second) {
		return -1;
	}
	return first > second;
}

/**
 * Compares two dates, ignoring the hour of the day.
 * @return -1 if the first is earlier, 1 if it is later, 0 if they are equal.
 */
int compare_dates(Date date1, Date date2) {
	Time midnight = {0, 0};
	return compare_date_time(date1, midnight, date2, midnight);
}

/**
 * Verifies that an instant may be registered, that is, that it exists and
 * that it is not earlier than the last entry or exit of the system.
 * @param sys state of the system
 * @return 1 if the instant may be registered, 0 otherwise.
 */
int valid_event_time(System *sys, Date date, Time time) {
	if (!valid_date_time(date, time)) {
		return 0;
	}
	if (!sys->has_events) {
		return 1;
	}
	return compare_date_time(date, time, sys->last_date,
				sys->last_time) >= 0;
}

/**
 * Stores the instant of the most recent entry or exit of the system.
 */
void set_last_event(System *sys, Date date, Time time) {
	sys->last_date = date;
	sys->last_time = time;
	sys->has_events = 1;
}
