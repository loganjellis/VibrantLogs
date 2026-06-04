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
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");

	timey_datetime now = timey_curr_datetime();
	timey_datetime future = timey_future_datetime(&now, 0, 0, 0, 0, 0, 10);
	vl_schedule_log_dt(VL_INFO, &future, "10 seconds after!");

	while(1)
	{
		vl_update(0.5);
	}

	return 0;
}
