#include "vibrant_logs.h"

int main(void)
{
	// init VibrantLogs!
	vl_init();

	// update config to print both time and date
	vl_config *vl_cfg = vl_curr_config();
	vl_cfg -> colors.time_color = (vl_color) { 150, 150, 255 };
	vl_cfg -> colors.date_color = (vl_color) { 175, 175, 255 };
	vl_cfg -> colors.debug_prefix_color = (vl_color) { 200, 100, 100 };
	vl_cfg -> colors.debug_color = (vl_color) { 225, 150, 150 };

	vl_cfg -> print_time = true;
	vl_cfg -> print_date = true;

	// testing each log type:
	vl_log(VL_INFO, "Info test.\n");
	vl_log(VL_SUCCESS, "Success!\n");
	vl_log(VL_DEBUG, "Debugging...\n");
	vl_log(VL_WARNING, "Warning.\n");
	vl_log(VL_ERROR, "Encountered an error!");

	return 0;
}
