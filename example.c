#include "vibrant_logs.h"

int main(void)
{
	// init VibrantLogs!
	vl_init();

	// you can edit the color scheme of VibrantLogs:
	vl_color_scheme *vl_colors = vl_curr_colors();
	vl_colors -> time_color = (vl_color) { 150, 150, 255 };
	vl_colors -> date_color = (vl_color) { 175, 175, 255 };
	vl_colors -> debug_prefix_color = (vl_color) { 200, 100, 100 };
	vl_colors -> debug_color = (vl_color) { 225, 150, 150 };

	// update config to print both time and date
	vl_config *vl_cfg = vl_curr_config();
	vl_cfg -> print_time = true;
	vl_cfg -> print_date = true;

	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");

	return 0;
}
