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

	// do not print time
	vl_config *vl_cfg = vl_curr_config();
	vl_cfg -> print_time = true;
	vl_cfg -> print_date = true;

	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");

	vl_timestamp now = vl_get_timestamp();
	vl_log(VL_INFO, "now : %hhu:%hhu:%hhu", now.hours, now.minutes, now.seconds);

	vl_timestamp future = vl_get_future_timestamp(&now, 10, 10, 10);
	vl_log(VL_INFO, "future time: %hhu:%hhu:%hhu", future.hours, future.minutes, future.seconds);

	// test out date
	char date[13];
	vl_get_date(date, sizeof date);

	vl_log(VL_INFO, "the date is %s", date);

	return 0;
}
