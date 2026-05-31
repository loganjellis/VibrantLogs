#include "vibrant_logs.h"

int main(void)
{
	// init VibrantLogs!
	vl_init();

	// you can edit the color scheme of VibrantLogs:
	vl_color_scheme mycolors = VL_DEFAULT_COLORS;
	mycolors.time_color = (vl_color) { 150, 150, 255 };
	mycolors.debug_prefix_color = (vl_color) { 200, 100, 100 };
	mycolors.debug_color = (vl_color) { 225, 150, 150 };

	// remember to set the color scheme using vl_set_colors(color_scheme):
	vl_set_colors(mycolors);

	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");

	vl_timestamp now = vl_get_timestamp();
	vl_log(VL_INFO, "now : %hhu:%hhu:%hhu", now.hour, now.minute, now.second);

	vl_timestamp future = vl_get_future_timestamp(&now, 10000, 1000, 1000000000);
	vl_log(VL_INFO, "future time: %hhu:%hhu:%hhu", future.hour, future.minute, future.second);

	return 0;
}
