#include "vibrant_logs.h"

int main(void)
{
	// you can edit the color scheme of VibrantLogs:
	viblogs_color_scheme mycolors = VIB_LOGS_DEFAULT_COLORS;
	mycolors.time_color = (viblogs_color) { 100, 150, 200 };
	mycolors.debug_prefix_color = (viblogs_color) { 200, 100, 100 };
	mycolors.debug_color = (viblogs_color) { 225, 150, 150 };

	// remember to set the color scheme using viblogs_set_colors(color_scheme):
	viblogs_set_colors(mycolors);

	// testing each log type:
	viblogs_log(VIB_LOGS_INFO, "Info test.");
	viblogs_log(VIB_LOGS_SUCCESSFUL_OPERATION, "Success!");
	viblogs_log(VIB_LOGS_DEBUG, "Debugging...");
	viblogs_log(VIB_LOGS_WARNING, "Warning.");
	viblogs_log(VIB_LOGS_ERROR, "ERROR!");
}
