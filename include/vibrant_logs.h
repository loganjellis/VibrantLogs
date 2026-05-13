#pragma once

/**
  Used to indicate a function has failed in some way.
*/
#define VIB_LOGS_FAIL 0
/**
  Used to indicate a function has succeeded.
*/
#define VIB_LOGS_SUCCESS 1

/**
  Represents the type of a log statement.
*/
typedef enum
{
	/**
	  Used for normal log messages.
	*/
	VIB_LOGS_INFO,
	/**
	  Used to signal that an operation
	  was successful.
	*/
	VIB_LOGS_SUCCESSFUL_OPERATION,
	/**
	  Used to debug a value or expression.
	*/
	VIB_LOGS_DEBUG,
	/**
	  Used to send a warning message.
	*/
	VIB_LOGS_WARNING,
	/**
	  Used to send an error message.
	*/
	VIB_LOGS_ERROR
} vib_logs_type;

/**
  Represents a color using values of 0 to 255.
*/
typedef struct vib_logs_color
{
	/**
	  The red value.
	*/
	int r;
	/**
	  The green value.
	*/
	int g;
	/**
	  The blue value.
	*/
	int b;
} vib_logs_color;

/**
  White.
*/
#define VIB_LOGS_WHITE (vib_logs_color) { 255, 255, 255 }
/**
  Light blue.
*/
#define VIB_LOGS_LIGHT_BLUE (vib_logs_color) { 0, 125, 215 }
/**
  Blue.
*/
#define VIB_LOGS_BLUE (vib_logs_color) { 0, 255, 255 }
/**
  Light green.
*/
#define VIB_LOGS_LIGHT_GREEN (vib_logs_color) { 0, 175, 0 }
/**
  Green.
*/
#define VIB_LOGS_GREEN (vib_logs_color) { 0, 255, 0 }
/**
  Light orange.
*/
#define VIB_LOGS_LIGHT_ORANGE (vib_logs_color) { 200, 125, 0 }
/**
  Orange.
*/
#define VIB_LOGS_ORANGE (vib_logs_color) { 255, 150, 0 }
/**
  Light yellow.
*/
#define VIB_LOGS_LIGHT_YELLOW (vib_logs_color) { 200, 200, 0 }
/**
  Yellow.
*/
#define VIB_LOGS_YELLOW (vib_logs_color) { 255, 255, 0 }
/**
  Light red.
*/
#define VIB_LOGS_LIGHT_RED (vib_logs_color) { 200, 0, 0 }
/**
  Red.
*/
#define VIB_LOGS_RED (vib_logs_color) { 255, 0, 0 }

/**
  Represents the color scheme of VibrantLogs.

  @see VIB_LOGS_DEFAULT_COLORS
*/
typedef struct vib_logs_color_scheme
{
	/**
	  The color of the time.
	*/
	vib_logs_color time_color;
	/**
	  The color of the prefix of info messages.
	*/
	vib_logs_color info_prefix_color;
	/**
	  The main color of info messages.
	*/
	vib_logs_color info_color;
	/**
	  The color of the prefix of successful operation messages.
	*/
	vib_logs_color success_prefix_color;
	/**
	  The main color of successful operation messages.
	*/
	vib_logs_color success_color;
	/**
	  The color of the prefix of debug messages.
	*/
	vib_logs_color debug_prefix_color;
	/**
	  The main color of debug messages.
	*/
	vib_logs_color debug_color;
	/**
	  The color of the prefix of warning messages.
	*/
	vib_logs_color warning_prefix_color;
	/**
	  The main color of warning messages.
	*/
	vib_logs_color warning_color;
	/**
	  The color of the prefix of error messages.
	*/
	vib_logs_color error_prefix_color;
	/**
	  The main color of error messages.
	*/
	vib_logs_color error_color;
} vib_logs_color_scheme;

/**
  Obtains the default color scheme of VibrantLogs.
*/
#define VIB_LOGS_DEFAULT_COLORS (vib_logs_color_scheme) { \
		.time_color = VIB_LOGS_WHITE, \
		.info_prefix_color = VIB_LOGS_LIGHT_BLUE, \
		.info_color = VIB_LOGS_BLUE, \
		.success_prefix_color = VIB_LOGS_LIGHT_GREEN, \
		.success_color = VIB_LOGS_GREEN, \
		.debug_prefix_color = VIB_LOGS_LIGHT_ORANGE, \
		.debug_color = VIB_LOGS_ORANGE, \
		.warning_prefix_color = VIB_LOGS_LIGHT_YELLOW, \
		.warning_color = VIB_LOGS_YELLOW, \
		.error_prefix_color = VIB_LOGS_LIGHT_RED, \
		.error_color = VIB_LOGS_RED \
	};

#ifdef _WIN32
	#ifdef VIB_LOGS_EXPORTS
		#define VIB_LOGS_API __declspec(dllexport)
	#elif defined(VIB_LOGS_STATIC)
		#define VIB_LOGS_API
	#else
		#define VIB_LOGS_API __declspec(dllimport)
	#endif
#else
	#define VIB_LOGS_API
#endif

/**
  Sets the color scheme of VibrantLogs.

  The initial color scheme of VibrantLogs
  is equal to VIB_LOGS_DEFAULT_COLORS.

  @see VIB_LOGS_DEFAULT_COLORS
*/
VIB_LOGS_API void vib_logs_set_colors(vib_logs_color_scheme color_scheme);
/**
  Obtains the current color scheme of VibrantLogs.
*/
VIB_LOGS_API vib_logs_color_scheme *vib_logs_curr_colors(void);

/**
  Inserts the current time string into the given buffer.

  @param buffer The buffer to insert the time into.
  @param size The size of the buffer in bytes. This size
  value must be at least 12 bytes to view the full
  time string.

  @return Either VIB_LOGS_FAIL or VIB_LOGS_SUCCESS.
*/
VIB_LOGS_API int vib_logs_get_time(char *buffer, size_t size);

/**
  Prints a message.

  @note The max length of a VibrantLogs message
  is 256 characters.

  @return Either VIB_LOGS_FAIL or VIB_LOGS_SUCCESS.
*/
VIB_LOGS_API int vib_logs_log(vib_logs_type log_type, const char *fmt, ...);
/**
  Prints a delayed message.

  @note The max length of a VibrantLogs message
  is 256 characters.

  @return Either VIB_LOGS_FAIL or VIB_LOGS_SUCCESS.
*/
VIB_LOGS_API int vib_logs_delay_log(vib_logs_type log_type, float seconds, const char *fmt, ...);
/**
  Updates the VibrantLogs library.

  This function is used to update delayed logs.
  It must be called in the program's event loop.
  The function also needs the delta time value
  of the program. Delta time is the time between
  the last frame and the current frame.
*/
VIB_LOGS_API void vib_logs_update(float delta_time);
