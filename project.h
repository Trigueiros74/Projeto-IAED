/**
 * Interface of the modules of the parking lot management system.
 * @file project.h
 * @author ist1110573
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <stddef.h>
#include "structures.h"

/* parse.c */
void *safe_alloc(size_t size);
char *read_token(char *line, char *token);
char *read_date_time(char *line, Date *date, Time *time);
char *read_date(char *line, Date *date);

/* dates.c */
int valid_date_time(Date date, Time time);
long date_to_minutes(Date date, Time time);
int compare_date_time(Date date1, Time time1, Date date2, Time time2);
int compare_dates(Date date1, Date date2);
int valid_event_time(System *sys, Date date, Time time);
void set_last_event(System *sys, Date date, Time time);

/* parking.c */
int find_park(System *sys, char *name);
void sort_parks_by_name(System *sys, Park *sorted[]);
void free_park(Park *park);
void command_park(System *sys, char *args);
void command_remove_park(System *sys, char *args);

/* vehicle.c */
int valid_plate(char *plate);
Vehicle *find_vehicle(System *sys, char *plate);
void detach_registry(Registry *reg);
void command_entry(System *sys, char *args);
void command_exit(System *sys, char *args);
void command_list_vehicle(System *sys, char *args);
void free_vehicles(System *sys);

/* billing.c */
double parking_fee(Park *park, Registry *reg);
void bill_registry(Park *park, Registry *reg);
void free_billing(Park *park);
void command_billing(System *sys, char *args);

#endif
