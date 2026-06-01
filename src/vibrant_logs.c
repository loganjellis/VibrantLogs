#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "vibrant_logs.h"

// the max length of a VibrantLogs message (what the user enters, not what VibrantLogs inserts before or after messages)
#define VL_MAX_MSG_LEN 256
// the max number of delayed log messages
#define VL_MAX_DELAY_MSGS 64
// enough chars to fit the time string in VibrantLogs ('[HH:MM:SS]') (includes null terminator)
#define VL_TIME_STR_LEN 11
// enough chars to fit the date string in VibrantLogs ('[MM-DD-YYYY]') (includes null terminator)
#define VL_DATE_STR_LEN 13

typedef struct vl_delayed_msg
{
	// the message to print
	char msg[VL_MAX_MSG_LEN + 1];
	// the log type
	vl_type type;
	// the amount of seconds before the message prints
	double remaining_time_sec;
} vl_delayed_msg;

// storage of all delayed logs:
static vl_delayed_msg delayed_msgs[VL_MAX_DELAY_MSGS];
static size_t delayed_msg_count = 0;

static bool vl_is_init = false;
static FILE *output_dest = NULL;
static vl_type log_level = VL_INFO;
static vl_color_scheme colors = VL_DEFAULT_COLORS;
static bool use_colors = true; // whether or not messages should be printed in color (only applies to stdout/stderr)
static vl_config config = {0};

// macro for quickly obtaining the RGB value of a color (used when printing to shorten lines)
#define vl_rgb(color) color.r, color.g, color.b

// determine if output_dest is a file/terminal
static bool vl_can_use_colors(void)
{
	#ifdef _WIN32
		DWORD mode;
		HANDLE h = (HANDLE) _get_osfhandle(_fileno(output_dest));

		if(h == INVALID_HANDLE_VALUE)
			return false;

		return GetConsoleMode(h, &mode) != 0;
	#else
		return isatty(fileno(output_dest));
	#endif
}

// print a message:
static void vl_print(vl_type type, const char *msg)
{
	// see if output_dest is NULL, indicating vl_init() was never called
	if(!output_dest)
	{
		// no check for 'use_colors' here since printf uses stdout by default
		printf("\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.error_prefix_color), vl_rgb(colors.error_color), "Output destination is NULL, make sure you call vl_init()!");
		return;
	}

	// if 'print_time' is true in the config, obtain and format time string:
	if(config.print_time)
	{
		// obtain the time string
		char time_buf[VL_TIME_STR_LEN];
		vl_get_time(time_buf, sizeof time_buf);

		// obtain formatted time string
		char time_str[32]; // time string (11 chars) + format string (21 chars) = 32
		if(use_colors)
			vl_get_str(time_str, sizeof time_str, "\x1b[38;2;%d;%d;%dm%s ", vl_rgb(colors.time_color), time_buf);
		else
			vl_get_str(time_str, sizeof time_str, "%s ", time_buf);

		// write time string to output
		fwrite(time_str, sizeof(char), strlen(time_str), output_dest);
	}

	// if 'print_date' is true in the config, obtain and format date string:
	if(config.print_date)
	{
		// obtain the date string
		char date_buf[VL_DATE_STR_LEN];
		vl_get_date(date_buf, sizeof date_buf);

		// obtain formatted date string
		char date_str[34]; // date string (13 chars) + format string (21 chars) = 34
		if(use_colors)
			vl_get_str(date_str, sizeof date_str, "\x1b[38;2;%d;%d;%dm%s ", vl_rgb(colors.date_color), date_buf);
		else
			vl_get_str(date_str, sizeof date_str, "%s ", date_buf);

		// write date string to output
		fwrite(date_str, sizeof(char), strlen(date_str), output_dest);
	}

	// buffer holding final log message (use VL_MAX_MSG_LEN + 56 to fit color codes, prefixes, and null terminator)
	char final_str[VL_MAX_MSG_LEN + 56];

	switch(type)
	{
		case VL_INFO:
			// if log level is VL_WARNING or VL_ERROR, do not print
			if(log_level == VL_WARNING || log_level == VL_ERROR)
				return;
			if(use_colors)
				vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ INFO ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.info_prefix_color), vl_rgb(colors.info_color), msg);
			else
				vl_get_str(final_str, sizeof final_str, "[ INFO ] %s\n", msg);
			break;
		case VL_SUCCESS:
			if(use_colors)
				vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ SUCCESS ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.success_prefix_color), vl_rgb(colors.success_color), msg);
			else
				vl_get_str(final_str, sizeof final_str, "[ SUCCESS ] %s\n", msg);
			break;
		case VL_DEBUG:
			if(use_colors)
				vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ DEBUG ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.debug_prefix_color), vl_rgb(colors.debug_color), msg);
			else
				vl_get_str(final_str, sizeof final_str, "[ DEBUG ] %s\n", msg);
			break;
		case VL_WARNING:
			// if log level is VL_ERROR, do not print
			if(log_level == VL_ERROR)
				return;
			if(use_colors)
				vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ WARNING ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.warning_prefix_color), vl_rgb(colors.warning_color), msg);
			else
				vl_get_str(final_str, sizeof final_str, "[ WARNING ] %s\n", msg);
			break;
		case VL_ERROR:
		default:
			if(use_colors)
				vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(colors.error_prefix_color), vl_rgb(colors.error_color), msg);
			else
				vl_get_str(final_str, sizeof final_str, "[ ERROR ] %s\n", msg);
			break;
	}

	// write final string to output
	fwrite(final_str, sizeof(char), strlen(final_str), output_dest);
}

int vl_init()
{
	if(vl_is_init)
		return 0;

	output_dest = stdout;

	// setup default config
	config.print_time = true;
	config.print_date = false;

	vl_is_init = true;

	return 1;
}

int vl_set_output_destination(FILE *dest)
{
	if(!dest)
		return 0;

	output_dest = dest;

	// determine if colors should be used based on new output destination
	use_colors = vl_can_use_colors();

	return 1;
}

void vl_set_log_level(vl_type level)
{
	log_level = level;
}

void vl_set_colors(vl_color_scheme color_scheme)
{
	colors = color_scheme;
}
vl_color_scheme *vl_curr_colors()
{
	return &colors;
}

void vl_use_config(vl_config cfg)
{
	config = cfg;
}
vl_config *vl_curr_config()
{
	return &config;
}

int vl_get_time(char *buffer, size_t size)
{
	if(!buffer)
		return 0;

	// time info:
	time_t raw_time = 0;
	struct tm *time_info;
	
	// obtain curr time
	time(&raw_time);

	// convert to local time
	time_info = localtime(&raw_time);

	// copy string into buffer
	strftime(buffer, size, "[%H:%M:%S]", time_info);

	return 1;
}
int vl_get_date(char *buffer, size_t size)
{
	if(!buffer)
		return 0;

	// date info:
	time_t raw_time = 0;
	struct tm *time_info;

	// obtain curr time
	time(&raw_time);

	// convert to local time
	time_info = localtime(&raw_time);

	// copy string into buffer
	strftime(buffer, size, "[%m-%d-%Y]", time_info);

	return 1;
}
int vl_get_str(char *buffer, size_t size, const char *fmt, ...)
{
	if(!buffer)
		return 0;

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, size, fmt, args);
	va_end(args);

	return 1;
}

vl_timestamp vl_get_timestamp()
{
	vl_timestamp ts = {0};

	// obtain time string
	char time[VL_TIME_STR_LEN];
	vl_get_time(time, sizeof time);

	// parse hour, min, and sec from time string: ('[HH:MM:SS]')
	char hour[3], min[3], sec[3]; // sizes must 3 to include '\0'

	// copy substrings into respective buffers
	memcpy(hour, time + 1, 2);
	hour[2] = '\0';

	memcpy(min, time + 4, 2);
	min[2] = '\0';

	memcpy(sec, time + 7, 2);
	sec[2] = '\0';

	ts.hour = strtoul(hour, NULL, 10);
	ts.minute = strtoul(min, NULL, 10);
	ts.second = strtoul(sec, NULL, 10);

	return ts;
}
// going to next complete time (either next hour or next minute, or both)
static void vl_increment_timestamp(vl_timestamp *ts)
{
	ts -> minute -= 60;
	ts -> hour += 1;
	if(ts -> hour > 24)
		ts -> hour -= 24;
}
vl_timestamp vl_get_future_timestamp(const vl_timestamp *now, unsigned int hours, unsigned int min, unsigned int sec)
{
	if(!now)
	{
		vl_log(VL_ERROR, "Cannot get future timestamp, 'now' is null.");
		return (vl_timestamp) {0};
	}

	vl_timestamp future = *now;

	// add hours, min, and sec to now

	// first add hours to now.hour
	future.hour += hours;
	// then, as long as now.hour exceeds 24, subtract 24 to loop back
	while(future.hour > 24)
		future.hour -= 24;

	// now add minutes
	future.minute += min;
	/* then, as long as now.minute exceeds 60, subtract 60 to loop back,
	and also increment now.hour (looping hour back as needed) */
	while(future.minute > 60)
		vl_increment_timestamp(&future);

	// now add seconds
	future.second += sec;
	/* then, as long as now.second exceeds 60, subtract 60 to loop back,
	and alsoincrement now.minute (looping back min as needed, and
	also checking for a new hour reached if now.minute reaches 60). */
	while(future.second > 60)
	{
		future.second -= 60;
		future.minute += 1;
		if(future.minute > 60)
			vl_increment_timestamp(&future);
	}

	return future;
}

// figure out if a year is a leap year
static bool vl_is_leap_year(unsigned int year)
{
	return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}
vl_datetime vl_get_datetime()
{
	vl_datetime dt = {0};

	// obtain date string
	char date[VL_DATE_STR_LEN];
	vl_get_date(date, sizeof date);

	// parse year, month, and day from date string: ('[MM-DD-YYYY]')
	char year[5], month[3], day[3];

	// copy substrings into respective buffers
	memcpy(month, date + 1, 2);
	month[2] = '\0';

	memcpy(day, date + 4, 2);
	day[2] = '\0';

	memcpy(year, date + 7, 4);
	year[4] = '\0';

	dt.year = strtoul(year, NULL, 10);
	dt.month = strtoul(month, NULL, 10);
	dt.day = strtoul(day, NULL, 10);
	// determine if the year is a leap year
	dt.is_leap_year = vl_is_leap_year(dt.year);

	// get timestamp
	dt.timestamp = vl_get_timestamp();

	return dt;
}
// get max day count for month in a date-time
static unsigned int vl_num_days_in_month(vl_datetime *dt)
{
	if(dt -> month == 1 || dt -> month == 3 || dt -> month == 5
			|| dt -> month == 7 || dt -> month == 8 || dt -> month == 10
			|| dt -> month == 12)
		return 31;
	else if(dt -> month == 2)
		return dt -> is_leap_year ? 29 : 28;
	else
		return 30;
}
// going to next complete date-time (either next year or next month, or both)
static void vl_increment_datetime(vl_datetime *dt)
{
	dt -> month -= 12;
	dt -> year += 1;
	// re-determine if year is a leap year
	dt -> is_leap_year = vl_is_leap_year(dt -> year);
}
// adding just days to a date-time
static void vl_add_days_datetime(vl_datetime *dt, unsigned int days)
{
	dt -> day += days;
	unsigned int max_month_days = vl_num_days_in_month(dt);
	while(dt -> day > max_month_days)
	{
		dt -> day -= max_month_days;
		dt -> month += 1;
		if(dt -> month > 12)
			vl_increment_datetime(dt);
		// re-calculate max_month_days
		max_month_days = vl_num_days_in_month(dt);
	}
}
vl_datetime vl_get_future_datetime(const vl_datetime *now, unsigned int years, unsigned int months, unsigned int days, unsigned int hours, unsigned int min, unsigned int sec)
{
	if(!now)
	{
		vl_log(VL_ERROR, "Cannot get future date-time, 'now' is null.");
		return (vl_datetime) {0};
	}

	vl_datetime future = *now;

	// add years, months, and days to now

	// first add years to now.year
	future.year += years;
	
	// now add months
	future.month += months;
	/* then as long as future.months exceeds 12, subtract 12 to loop back,
	and also increment future.years */
	while(future.month > 12)
		vl_increment_datetime(&future);

	// now add days
	vl_add_days_datetime(&future, days);

	// add to time (while tracking new days, months, and years)
	vl_timestamp *fts = &future.timestamp;

	// first add hours
	fts -> hour += hours;
	// then, as long as future.hour exceeds 24, subtract 24 to loop back, and increase day
	while(fts -> hour > 24)
	{
		fts -> hour -= 24;
		vl_add_days_datetime(&future, 1);
	}

	// then add min
	fts -> minute += min;
	// then, as long as future.minute exceeds 60, subtract 60 to loop back, and increase hour (check for new day as well)
	while(fts -> minute > 60)
	{
		fts -> minute -= 60;
		fts -> hour += 1;
		if(fts -> hour > 24)
		{
			fts -> hour -= 24;
			vl_add_days_datetime(&future, 1);
		}
	}

	// then add sec
	fts -> second += sec;
	// then, as long as future.second exceeds 60, subtract 60 to loop back, and increase minute (check for new hour/new day as well)
	while(fts -> second > 60)
	{
		fts -> second -= 60;
		fts -> minute += 1;
		if(fts -> minute > 60)
		{
			fts -> minute -= 60;
			fts -> hour += 1;
			if(fts -> hour > 24)
			{
				fts -> hour -= 24;
				vl_add_days_datetime(&future, 1);
			}
		}
	}

	return future;
}

int vl_log(vl_type log_type, const char *fmt, ...)
{
	// check for null message:
	if(!fmt)
		return 0;

	// buffer to store message
	char buf[VL_MAX_MSG_LEN + 1];

	// populate buffer:
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	// print the message immediately
	vl_print(log_type, buf);

	return 1;
}
int vl_delay_log(vl_type log_type, double seconds, const char *fmt, ...)
{
	// check for invalid args:
	if(!fmt || seconds < 0.0)
		return 0;

	// avoid overflow
	if(delayed_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	// temp buffer for formatted string
	char buf[VL_MAX_MSG_LEN + 1];

	// populate buffer:
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	// avoid duplicate messages
	for(size_t i = 0; i < delayed_msg_count; ++i)
	{
		vl_delayed_msg *existing = &delayed_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	// obtain the current vl_delayed_msg and modify it:
	vl_delayed_msg *msg = &delayed_msgs[delayed_msg_count];
	msg -> type = log_type;
	msg -> remaining_time_sec = seconds;

	// copy the buffer message but into the current vl_delayed_msg
	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';

	// move to next delayed message
	delayed_msg_count++;

	return 1;
}
void vl_update(double delta_time)
{
	for(size_t i = 0; i < delayed_msg_count;)
	{
		// get current message to work on
		vl_delayed_msg *msg = &delayed_msgs[i];

		// update timer
		msg -> remaining_time_sec -= delta_time;

		// check to see if that message's time has passed
		if(msg -> remaining_time_sec <= 0.0)
		{
			vl_print(msg -> type, msg -> msg);

			delayed_msgs[i] = delayed_msgs[--delayed_msg_count];
		}
		else
			i++;
	}
}
