#include "vibrant_logs.h"

int main(void)
{
	// you can edit the color scheme of VibrantLogs:
	vl_color_scheme mycolors = VL_DEFAULT_COLORS;
	mycolors.time_color = (vl_color) { 150, 150, 255 };
	mycolors.debug_prefix_color = (vl_color) { 200, 100, 100 };
	mycolors.debug_color = (vl_color) { 225, 150, 150 };

	// remember to set the color scheme using vl_set_colors(color_scheme):
	vl_set_colors(mycolors);


	// test out different log levels:

	// vl_set_log_level(VL_INFO); // default log level, shows all messages
	// vl_set_log_level(VL_WARNING); // shows warnings and errors
	// vl_set_log_level(VL_ERROR); // shows only errors

	// note that VL_DEBUG and VL_SUCCESS have no effect when used in vl_set_log_level(...).


	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");
}
