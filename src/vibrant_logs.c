#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "vibrant_logs.h"

// the max length of a VibrantLogs message
#define VL_MAX_MSG_LEN 256
// the max number of delayed log messages
#define VL_MAX_DELAY_MSGS 64

// used to represent a single message dispatched by VibrantLogs
typedef struct vl_msg
{
	// the message to print
	char msg[VL_MAX_MSG_LEN];
	// the log type
	vl_type type;
	// the amount of seconds before the message prints
	float timer;
	// the number of seconds this message is delayed by
	float delay;
} vl_msg;

// storage of all logs:
static vl_msg delayed_msgs[VL_MAX_DELAY_MSGS];
static size_t delayed_msg_count = 0;

// color scheme (init to default color scheme)
static vl_color_scheme vl_colors = VL_DEFAULT_COLORS;

// macro for quickly obtaining the RGB value of a color (used when printing to shorten lines)
#define vl_rgb(color) color.r, color.g, color.b

static vl_type log_level = VL_INFO;

static void vl_print(vl_type type, const char *msg)
{
	// obtain the time string
	char time_buf[12];
	vl_get_time(time_buf, sizeof time_buf);

	// print time string
	printf("\x1b[38;2;%d;%d;%dm%s ", vl_rgb(vl_colors.time_color), time_buf);

	switch(type)
	{
		case VL_INFO:
			// if log level is VL_WARNING or VL_ERROR, do not print
			if(log_level == VL_WARNING || log_level == VL_ERROR)
				break;
			printf("\x1b[38;2;%d;%d;%dm[ INFO ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.info_prefix_color), vl_rgb(vl_colors.info_color), msg);
			break;
		case VL_SUCCESS:
			printf("\x1b[38;2;%d;%d;%dm[ SUCCESS ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.success_prefix_color), vl_rgb(vl_colors.success_color), msg);
			break;
		case VL_DEBUG:
			printf("\x1b[38;2;%d;%d;%dm[ DEBUG ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.debug_prefix_color), vl_rgb(vl_colors.debug_color), msg);
			break;
		case VL_WARNING:
			// if log level is VL_ERROR, do not print
			if(log_level == VL_ERROR)
				break;
			printf("\x1b[38;2;%d;%d;%dm[ WARNING ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.warning_prefix_color), vl_rgb(vl_colors.warning_color), msg);
			break;
		case VL_ERROR:
			printf("\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.error_prefix_color), vl_rgb(vl_colors.error_color), msg);
			break;
		default:
			printf("\x1b[38;2;%d;%d;%dm[ ERROR ] \x1b[38;2;%d;%d;%dm%s\x1b[0m\n", vl_rgb(vl_colors.error_prefix_color), vl_rgb(vl_colors.error_color), "Invalid log type.");
			break;
	}
}

void vl_set_log_level(vl_type level)
{
	log_level = level;
}

void vl_set_colors(vl_color_scheme color_scheme)
{
	vl_colors = color_scheme;
}
vl_color_scheme *vl_curr_colors()
{
	return &vl_colors;
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

int vl_log(vl_type log_type, const char *fmt, ...)
{
	// check for null message:
	if(!fmt)
		return 0;

	// buffer to store message
	char buf[VL_MAX_MSG_LEN];

	// populate buffer:
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, VL_MAX_MSG_LEN, fmt, args);
	va_end(args);

	// print the message immediately
	vl_print(log_type, buf);

	return 1;
}
int vl_delay_log(vl_type log_type, float seconds, const char *fmt, ...)
{
	// check for invalid args:
	if(!fmt || seconds < 0.0f)
		return 0;

	// avoid overflow
	if(delayed_msg_count >= VL_MAX_DELAY_MSGS)
		return 0;

	// temp buffer for formatted string
	char buf[VL_MAX_MSG_LEN];

	// populate buffer:
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, VL_MAX_MSG_LEN, fmt, args);
	va_end(args);

	// avoid duplicate messages
	for(size_t i = 0; i < delayed_msg_count; ++i)
	{
		vl_msg *existing = &delayed_msgs[i];
		if(existing -> type == log_type && strcmp(existing -> msg, buf) == 0)
			return 0;
	}

	// obtain the current vl_msg and modify it:
	vl_msg *msg = &delayed_msgs[delayed_msg_count];
	msg -> type = log_type;
	msg -> timer = seconds;
	msg -> delay = seconds;

	// re-print the buffer message but into the current vl_msg
	snprintf(msg -> msg, VL_MAX_MSG_LEN, "%s", buf);

	// move to next delayed message
	delayed_msg_count++;

	return 1;
}
void vl_update(float delta_time)
{
	for(size_t i = 0; i < delayed_msg_count;)
	{
		// get current message to work on
		vl_msg *msg = &delayed_msgs[i];

		// update timer
		msg -> timer -= delta_time;

		// check to see if that message's time has passed
		if(msg -> timer <= 0.0f)
		{
			vl_print(msg -> type, msg -> msg);

			delayed_msg_count--;
			if(i < delayed_msg_count)
				delayed_msgs[i] = delayed_msgs[delayed_msg_count];
		}
		else
			i++;
	}
}
