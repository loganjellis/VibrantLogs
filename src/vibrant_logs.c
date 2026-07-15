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

#include "timey.h"
#include "vibrant_logs.h"

// the max length of a VibrantLogs message (what the user enters, not what VibrantLogs inserts before or after messages)
#define VL_MAX_MSG_LEN 256
// the max number of delayed log messages
#define VL_MAX_DELAY_MSGS 64
// enough chars to fit the time string in VibrantLogs ('[HH:MM:SS]') (includes null terminator)
#define VL_TIME_STR_LEN 11
// enough chars to fit the date string in VibrantLogs ('[MM-DD-YYYY]' or '[MONTH-DD-YYYY]') (includes null terminator)
#define VL_DATE_STR_LEN 20

typedef struct vl_delayed_msg
{
	// the message to print
	char msg[VL_MAX_MSG_LEN + 1];
	// the amount of seconds before the message prints
	double remaining_time_sec;
	// the log type
	vl_type type;
	// should the message label be printed?
	bool print_label;
} vl_delayed_msg;

typedef struct vl_scheduled_msg
{
	// msg to print
	char msg[VL_MAX_MSG_LEN + 1];
	// date-time
	timey_datetime dt;
	// time stamp
	timey_timestamp ts;
	// log type
	vl_type type;
	// if this msg was scheduled using a timestamp only
	bool scheduled_ts;
	// should the message label be printed?
	bool print_label;
} vl_scheduled_msg;

// storage of all delayed logs:
static vl_delayed_msg delayed_msgs[VL_MAX_DELAY_MSGS];
static size_t delayed_msg_count = 0;

// storage of all scheduled logs:
static vl_scheduled_msg scheduled_msgs[VL_MAX_DELAY_MSGS];
static size_t scheduled_msg_count = 0;

static bool vl_is_init = false;
static vl_config config = {0};

// macro for quickly obtaining the RGB value of a color (used when printing to shorten lines)
#define vl_rgb(color) color.r, color.g, color.b

// determine if config.output_destination is a file/terminal
static bool vl_can_use_colors(void)
{
	#ifdef _WIN32
		DWORD mode;
		HANDLE h = (HANDLE) _get_osfhandle(_fileno(config.output_destination));

		if(h == INVALID_HANDLE_VALUE)
			return false;

		return GetConsoleMode(h, &mode) != 0;
	#else
		return isatty(fileno(config.output_destination));
	#endif
}

// TODO module later
static void vl_get_str(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, size, fmt, args);
	va_end(args);
}

// print a message:
static void vl_print_msg(vl_type type, const char *msg, bool print_label, bool print_time, bool print_date)
{
	// see if config.output_destination is NULL, indicating vl_init() was never called
	if(!config.output_destination)
	{
		// no check for 'use_colors' here since printf uses stdout by default
		if(print_label)
			printf("\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.error_prefix_color), vl_rgb(config.colors.error_color), "Output destination is NULL, make sure you call vl_init()!");
		else
			printf("\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.error_prefix_color), vl_rgb(config.colors.error_color), "Output destination is NULL, make sure you call vl_init()!");
		return;
	}

	bool use_colors = vl_can_use_colors();

	// check for log level and the type given
	if(type == VL_INFO && (config.log_level == VL_WARNING || config.log_level == VL_ERROR))
		return;
	if(type == VL_WARNING && config.log_level == VL_ERROR)
		return;

	// if 'print_time' is true, obtain and format time string:
	if(print_time)
	{
		// obtain the time string
		char time_buf[VL_TIME_STR_LEN];
		timey_query_time(time_buf, sizeof time_buf);

		// obtain formatted time string
		char time_str[32]; // time string (11 chars) + format string (21 chars) = 32
		if(use_colors)
			vl_get_str(time_str, sizeof time_str, "\x1b[38;2;%d;%d;%dm%s ", vl_rgb(config.colors.time_color), time_buf);
		else
			vl_get_str(time_str, sizeof time_str, "%s ", time_buf);

		// write time string to output
		fwrite(time_str, sizeof(char), strlen(time_str), config.output_destination);
	}

	// if 'print_date' is true, obtain and format date string:
	if(print_date)
	{
		// obtain the date string
		char date_buf[VL_DATE_STR_LEN];
		timey_query_date(date_buf, sizeof date_buf);

		// obtain formatted date string
		char date_str[34]; // date string (13 chars) + format string (21 chars) = 34
		if(use_colors)
			vl_get_str(date_str, sizeof date_str, "\x1b[38;2;%d;%d;%dm%s ", vl_rgb(config.colors.date_color), date_buf);
		else
			vl_get_str(date_str, sizeof date_str, "%s ", date_buf);

		// write date string to output
		fwrite(date_str, sizeof(char), strlen(date_str), config.output_destination);
	}

	// buffer holding final log message (use VL_MAX_MSG_LEN + 56 to fit color codes, prefixes, and null terminator)
	char final_str[VL_MAX_MSG_LEN + 56];

	switch(type)
	{
		case VL_INFO:
			if(print_label)
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ INFO ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.info_prefix_color), vl_rgb(config.colors.info_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "[ INFO ] %s", msg);
			}
			else
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.info_prefix_color), vl_rgb(config.colors.info_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "%s", msg);
			}
			break;
		case VL_SUCCESS:
			if(print_label)
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ SUCCESS ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.success_prefix_color), vl_rgb(config.colors.success_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "[ SUCCESS ] %s", msg);
			}
			else
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.success_prefix_color), vl_rgb(config.colors.success_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "%s", msg);
			}
			break;
		case VL_DEBUG:
			if(print_label)
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ DEBUG ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.debug_prefix_color), vl_rgb(config.colors.debug_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "[ DEBUG ] %s", msg);
			}
			else
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.debug_prefix_color), vl_rgb(config.colors.debug_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "%s", msg);
			}
			break;
		case VL_WARNING:
			if(print_label)
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ WARNING ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.warning_prefix_color), vl_rgb(config.colors.warning_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "[ WARNING ] %s", msg);
			}
			else
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.warning_prefix_color), vl_rgb(config.colors.warning_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "%s", msg);
			}
			break;
		case VL_ERROR:
		default:
			if(print_label)
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.error_prefix_color), vl_rgb(config.colors.error_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "[ ERROR ] %s", msg);
			}
			else
			{
				if(use_colors)
					vl_get_str(final_str, sizeof final_str, "\x1b[38;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", vl_rgb(config.colors.error_prefix_color), vl_rgb(config.colors.error_color), msg);
				else
					vl_get_str(final_str, sizeof final_str, "%s", msg);
			}
			break;
	}

	// write final string to output
	fwrite(final_str, sizeof(char), strlen(final_str), config.output_destination);
}

int vl_init()
{
	if(vl_is_init)
		return 0;

	// default config:
	config.colors = VL_DEFAULT_COLORS;
	config.output_destination = stdout;
	config.log_level = VL_INFO;
	config.print_time = true;
	config.print_date = false;
	config.print_month_name = false;

	vl_is_init = true;

	return 1;
}

void vl_use_config(vl_config cfg)
{
	config = cfg;
}
vl_config *vl_curr_config()
{
	return &config;
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
	vl_print_msg(log_type, buf, true, config.print_time, config.print_date);

	return 1;
}
int vl_print(vl_type log_type, const char *fmt, ...)
{
	if(!fmt)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	vl_print_msg(log_type, buf, false, false, false);

	return 1;
}

int vl_delay_log(vl_type log_type, double seconds, const char *fmt, ...)
{
	if(!fmt || seconds < 0.0)
		return 0;
	if(delayed_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < delayed_msg_count; ++i)
	{
		vl_delayed_msg *existing = &delayed_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_delayed_msg *msg = &delayed_msgs[delayed_msg_count];
	msg -> type = log_type;
	msg -> remaining_time_sec = seconds;
	msg -> print_label = true;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';
	delayed_msg_count++;

	return 1;
}
int vl_delay_print(vl_type log_type, double seconds, const char *fmt, ...)
{
	if(!fmt || seconds < 0.0)
		return 0;
	if(delayed_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < delayed_msg_count; ++i)
	{
		vl_delayed_msg *existing = &delayed_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_delayed_msg *msg = &delayed_msgs[delayed_msg_count];
	msg -> type = log_type;
	msg -> remaining_time_sec = seconds;
	msg -> print_label = false;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';
	delayed_msg_count++;

	return 1;	
}
int vl_schedule_log_ts(vl_type log_type, timey_timestamp *ts, const char *fmt, ...)
{
	if(!ts || !fmt)
		return 0;

	if(scheduled_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < scheduled_msg_count; ++i)
	{
		vl_scheduled_msg *existing = &scheduled_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_scheduled_msg *msg = &scheduled_msgs[scheduled_msg_count];
	msg -> type = log_type;
	msg -> ts = *ts;
	msg -> dt = (timey_datetime) {0};
	msg -> scheduled_ts = true;
	msg -> print_label = true;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';

	scheduled_msg_count++;

	return 1;
}
int vl_schedule_print_ts(vl_type log_type, struct timey_timestamp *ts, const char *fmt, ...)
{
	if(!ts || !fmt)
		return 0;

	if(scheduled_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < scheduled_msg_count; ++i)
	{
		vl_scheduled_msg *existing = &scheduled_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_scheduled_msg *msg = &scheduled_msgs[scheduled_msg_count];
	msg -> type = log_type;
	msg -> ts = *ts;
	msg -> dt = (timey_datetime) {0};
	msg -> scheduled_ts = true;
	msg -> print_label = false;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';

	scheduled_msg_count++;

	return 1;
}
int vl_schedule_log_dt(vl_type log_type, timey_datetime *dt, const char *fmt, ...)
{
	if(!dt || !fmt)
		return 0;

	if(scheduled_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < scheduled_msg_count; ++i)
	{
		vl_scheduled_msg *existing = &scheduled_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_scheduled_msg *msg = &scheduled_msgs[scheduled_msg_count];
	msg -> type = log_type;
	msg -> dt = *dt;
	msg -> ts = (timey_timestamp) {0};
	msg -> scheduled_ts = false;
	msg -> print_label = true;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';

	scheduled_msg_count++;

	return 1;
}
int vl_schedule_print_dt(vl_type log_type, timey_datetime *dt, const char *fmt, ...)
{
	if(!dt || !fmt)
		return 0;

	if(scheduled_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	char buf[VL_MAX_MSG_LEN + 1];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	for(size_t i = 0; i < scheduled_msg_count; ++i)
	{
		vl_scheduled_msg *existing = &scheduled_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	vl_scheduled_msg *msg = &scheduled_msgs[scheduled_msg_count];
	msg -> type = log_type;
	msg -> dt = *dt;
	msg -> ts = (timey_timestamp) {0};
	msg -> scheduled_ts = false;
	msg -> print_label = false;

	strncpy(msg -> msg, buf, VL_MAX_MSG_LEN);
	msg -> msg[VL_MAX_MSG_LEN] = '\0';

	scheduled_msg_count++;

	return 1;
}

void vl_update(double delta_time)
{
	// run through delayed messages:
	for(size_t i = 0; i < delayed_msg_count;)
	{
		// get current message to work on
		vl_delayed_msg *msg = &delayed_msgs[i];

		// update timer
		msg -> remaining_time_sec -= delta_time;

		// check to see if that message's time has passed
		if(msg -> remaining_time_sec <= 0.0)
		{
			vl_print_msg(msg -> type, msg -> msg, msg -> print_label, config.print_time, config.print_date);

			// remove this msg and replace it with the last element
			delayed_msgs[i] = delayed_msgs[--delayed_msg_count];
		}
		else
			i++;
	}

	// run through scheduled messages:
	for(size_t i = 0; i < scheduled_msg_count;)
	{
		// get current timestamp and datetime
		timey_timestamp now_ts = timey_curr_timestamp();
		timey_datetime now_dt = timey_curr_datetime();

		// get current message to work on
		vl_scheduled_msg *msg = &scheduled_msgs[i];

		// check its timestamp or datetime vs. now_ts and now_dt
		if(msg -> scheduled_ts)
		{
			// handle timestamp only

			// first see if same period
			if(strcmp(msg -> ts.period, now_ts.period) == 0)
			{
				// now match hours, min, and sec
				if(msg -> ts.hour12 == now_ts.hour12 && msg -> ts.min == now_ts.min && msg -> ts.sec == now_ts.sec)
				{
					vl_print_msg(msg -> type, msg -> msg, msg -> print_label, config.print_time, config.print_date);

					scheduled_msgs[i] = scheduled_msgs[--scheduled_msg_count];
				}
				else
					i++;
			}
		}
		else
		{
			// handle datetime

			// first see if same period
			if(strcmp(msg -> dt.time.period, now_dt.time.period) == 0)
			{
				if(msg -> dt.year == now_dt.year && msg -> dt.month == now_dt.month && msg -> dt.day == now_dt.day &&
						msg -> dt.time.hour12 == now_dt.time.hour12 && msg -> dt.time.min == now_dt.time.min &&
						msg -> dt.time.sec == now_dt.time.sec)
				{
					vl_print_msg(msg -> type, msg -> msg, true, config.print_time, config.print_date);

					scheduled_msgs[i] = scheduled_msgs[--scheduled_msg_count];
				}
				else
					i++;
			}
		}
	}
}
