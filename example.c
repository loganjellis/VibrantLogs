#include "vibrant_logs.h"

int main(void)
{
	// you can edit the color scheme of VibrantLogs:
	vib_logs_color_scheme mycolors = VIB_LOGS_DEFAULT_COLORS;
	mycolors.time_color = (vib_logs_color) { 100, 150, 200 };
	mycolors.debug_prefix_color = (vib_logs_color) { 200, 100, 100 };
	mycolors.debug_color = (vib_logs_color) { 225, 150, 150 };

	// remember to set the color scheme using vib_logs_set_colors(color_scheme):
	vib_logs_set_colors(mycolors);

	// testing each log type:
	vib_logs_log(VIB_LOGS_INFO, "Info test.");
	vib_logs_log(VIB_LOGS_SUCCESS, "Success!");
	vib_logs_log(VIB_LOGS_DEBUG, "Debugging...");
	vib_logs_log(VIB_LOGS_WARNING, "Warning.");
	vib_logs_log(VIB_LOGS_ERROR, "ERROR!");
}
