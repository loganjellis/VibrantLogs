#pragma once

#include <stdio.h>

/**
  Represents the type of a log statement.
*/
typedef enum
{
	/**
	  Used for normal log messages.
	*/
	VL_INFO,
	/**
	  Used to signal that an operation
	  was successful.
	*/
	VL_SUCCESS,
	/**
	  Used to debug a value or expression.
	*/
	VL_DEBUG,
	/**
	  Used to send a warning message.
	*/
	VL_WARNING,
	/**
	  Used to send an error message.
	*/
	VL_ERROR
} vl_type;

/**
  Represents a color using values of 0 to 255.
*/
typedef struct vl_color
{
	/**
	  The red value.
	*/
	unsigned char r;
	/**
	  The green value.
	*/
	unsigned char g;
	/**
	  The blue value.
	*/
	unsigned char b;
} vl_color;

/**
  White.
*/
#define VL_WHITE (vl_color) { 255, 255, 255 }
/**
  Light blue.
*/
#define VL_LIGHT_BLUE (vl_color) { 0, 125, 215 }
/**
  Blue.
*/
#define VL_BLUE (vl_color) { 0, 255, 255 }
/**
  Light green.
*/
#define VL_LIGHT_GREEN (vl_color) { 0, 175, 0 }
/**
  Green.
*/
#define VL_GREEN (vl_color) { 0, 255, 0 }
/**
  Light orange.
*/
#define VL_LIGHT_ORANGE (vl_color) { 200, 125, 0 }
/**
  Orange.
*/
#define VL_ORANGE (vl_color) { 255, 150, 0 }
/**
  Light yellow.
*/
#define VL_LIGHT_YELLOW (vl_color) { 200, 200, 0 }
/**
  Yellow.
*/
#define VL_YELLOW (vl_color) { 255, 255, 0 }
/**
  Light red.
*/
#define VL_LIGHT_RED (vl_color) { 200, 0, 0 }
/**
  Red.
*/
#define VL_RED (vl_color) { 255, 0, 0 }

/**
  Represents the color scheme of VibrantLogs.

  @see VL_DEFAULT_COLORS
*/
typedef struct vl_color_scheme
{
	/**
	  The color of the time (if printed).

	  @see vl_config.
	  @see vl_use_config(vl_config).
	*/
	vl_color time_color;
	/**
	  The color of the date (if printed).

	  @see vl_config.
	  @see vl_use_config(vl_config).
	*/
	vl_color date_color;
	/**
	  The color of the prefix of info messages.
	*/
	vl_color info_prefix_color;
	/**
	  The main color of info messages.
	*/
	vl_color info_color;
	/**
	  The color of the prefix of successful operation messages.
	*/
	vl_color success_prefix_color;
	/**
	  The main color of successful operation messages.
	*/
	vl_color success_color;
	/**
	  The color of the prefix of debug messages.
	*/
	vl_color debug_prefix_color;
	/**
	  The main color of debug messages.
	*/
	vl_color debug_color;
	/**
	  The color of the prefix of warning messages.
	*/
	vl_color warning_prefix_color;
	/**
	  The main color of warning messages.
	*/
	vl_color warning_color;
	/**
	  The color of the prefix of error messages.
	*/
	vl_color error_prefix_color;
	/**
	  The main color of error messages.
	*/
	vl_color error_color;
} vl_color_scheme;

/**
  Obtains the default color scheme of VibrantLogs.
*/
#define VL_DEFAULT_COLORS (vl_color_scheme) { \
		.time_color = VL_WHITE, \
		.date_color = VL_WHITE, \
		.info_prefix_color = VL_LIGHT_BLUE, \
		.info_color = VL_BLUE, \
		.success_prefix_color = VL_LIGHT_GREEN, \
		.success_color = VL_GREEN, \
		.debug_prefix_color = VL_LIGHT_ORANGE, \
		.debug_color = VL_ORANGE, \
		.warning_prefix_color = VL_LIGHT_YELLOW, \
		.warning_color = VL_YELLOW, \
		.error_prefix_color = VL_LIGHT_RED, \
		.error_color = VL_RED \
	};

/**
  Holds time data (hour, minute, and second).

  Each time component is an unsigned integer.
*/
typedef struct vl_timestamp
{
	/**
	  The time's hour.
	*/
	unsigned int hour;
	/**
	  The time's minute.
	*/
	unsigned int minute;
	/**
	  The time's second.
	*/
	unsigned int second;
} vl_timestamp;

/**
  Holds time data (hour, minute, and second),
  as well as date information (year, month, and day).

  Each time and date component is an unsigned integer.
*/
typedef struct vl_datetime
{
	/**
	  The timestamp.
	*/
	vl_timestamp timestamp;
	/**
	  The year.
	*/
	unsigned int year;
	/**
	  The month.
	*/
	unsigned int month;
	/**
	  The day of the month.
	*/
	unsigned int day;
	/**
	  Whether or not the year of this date-time
	  is a leap year.
	*/
	bool is_leap_year;
} vl_datetime;

/**
  Represents a custom config for VibrantLogs.

  When VibrantLogs logs messages, you can set
  how VibrantLogs prints the time and date.
*/
typedef struct vl_config
{
	/**
	  Indicates whether or not VibrantLogs will
	  print the current time. This is true by default.
	*/
	bool print_time;
	/**
	  Indicates whether or not VibrantLogs will
	  print the current date. This includes
	  the day, month, and year. This is false
	  by default.
	*/
	bool print_date;
} vl_config;

#ifdef _WIN32
	#ifdef VL_EXPORTS
		#define VL_API __declspec(dllexport)
	#else
		#define VL_API __declspec(dllimport)
	#endif
#else
	#define VL_API
#endif

/**
  Initializes the VibrantLogs library.

  @note This must be called for VibrantLogs
  to work properly.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_init(void);

/**
  Sets the location of the output from VibrantLogs.

  The default output location is stdout.

  @note VibrantLogs does not manage the FILE* passed
  in here, so any operations other than writing
  to it will have to be performed by the user.

  @important When writing to a destination outside
  of the terminal, such as a text file, VibrantLogs
  will not be able to print in color.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_set_output_destination(FILE *dest);

/**
  Sets the log level of VibrantLogs.

  Setting the log level will prevent VibrantLogs
  from printing certain log messages based on their
  type. For example, setting the log level to VL_WARNING
  means VibrantLogs can only print out warning and error
  messages. Setting it to VL_ERROR means only error messages
  can be printed. By default, the log level is set to
  VL_INFO, which means any type of message can be printed.

  @note VL_SUCCESS and VL_DEBUG are exceptions to this
  rule. No matter the log level of VibrantLogs, these
  types of messages will always print.
*/
VL_API void vl_set_log_level(vl_type level);

/**
  Sets the color scheme of VibrantLogs.

  The initial color scheme of VibrantLogs
  is equal to VL_DEFAULT_COLORS.

  @see VL_DEFAULT_COLORS
*/
VL_API void vl_set_colors(vl_color_scheme color_scheme);
/**
  Obtains the current color scheme of VibrantLogs.
*/
VL_API vl_color_scheme *vl_curr_colors(void);

/**
  Sets the config of VibrantLogs.

  @see vl_config.
*/
VL_API void vl_use_config(vl_config cfg);
/**
  Obtains the current config of VibrantLogs.
*/
VL_API vl_config *vl_curr_config(void);

/**
  Inserts the current time string into the given buffer.

  @param buffer The buffer to insert the time into.
  @param size The size of the buffer in bytes.

  @note For the buffer to hold the full time string, it should
  hold 11 bytes, including the null terminator.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_get_time(char *buffer, size_t size);
/**
  Inserts the current date string into the given buffer.

  @param buffer The buffer to insert the date into.
  @param size The size of the buffer in bytes.

  @note For the buffer to hold the full date string, it should
  hold 13 bytes, including the null terminator.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_get_date(char *buffer, size_t size);
/**
  Inserts a formatted string into a buffer.

  @param buffer The buffer to insert the formatted
  string into.
  @param size The size of the buffer in bytes.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_get_str(char *buffer, size_t size, const char *fmt, ...);

/**
  Obtains a timestamp based on the current time.
*/
VL_API vl_timestamp vl_get_timestamp(void);
/**
  Obtains a new timestamp in the future based
  on the given hours, minutes, and seconds.
*/
VL_API vl_timestamp vl_get_future_timestamp(const vl_timestamp *now, unsigned int hours, unsigned int min, unsigned int sec);

/**
  Obtains a date-time based on the current time and date.
*/
VL_API vl_datetime vl_get_datetime(void);
/**
  Obtains a new date-time in the future based on
  the given hours, minutes, and seconds.
*/
VL_API vl_datetime vl_get_future_datetime(const vl_datetime *now, unsigned int years, unsigned int months, unsigned int days, unsigned int hours, unsigned int min, unsigned int sec);

/**
  Prints a message.

  @note The max length of a VibrantLogs message
  is 256 characters.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_log(vl_type log_type, const char *fmt, ...);
/**
  Prints a delayed message.

  @note The max length of a VibrantLogs message
  is 256 characters.

  @return 1 on success, 0 on failure.
*/
VL_API int vl_delay_log(vl_type log_type, double seconds, const char *fmt, ...);
/**
  Updates the VibrantLogs library.

  This function is used to update delayed logs.
  It must be called in the program's event loop.
  The function also needs the delta time value
  of the program. Delta time is the time between
  the last frame and the current frame.
*/
VL_API void vl_update(double delta_time);
