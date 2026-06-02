#pragma once

/**
  Holds time data (hour, minute, and second).

  Each time component is an unsigned integer.
*/
typedef struct timey_timestamp
{
	/**
	  The time's hour.
	*/
	unsigned int hour;
	/**
	  The time's minute.
	*/
	unsigned int min;
	/**
	  The time's second.
	*/
	unsigned int sec;
} timey_timestamp;

/**
  Holds time data (hour, minute, and second),
  as well as date information (year, month, and day).

  Each component is an unsigned integer.
*/
typedef struct timey_datetime
{
	/**
	  The timestamp.
	*/
	timey_timestamp time;
	/**
	  The date's year.
	*/
	unsigned int year;
	/**
	  The date's month.
	*/
	unsigned int month;
	/**
	  The date's day.
	*/
	unsigned int day;
	/**
	  Whether or not the year of this date-time
	  is a leap year.
	*/
	bool is_leap_year;
} timey_datetime;

#ifdef _WIN32
	#ifdef TIMEY_EXPORTS
		#define TIMEY_API __declspec(dllexport)
	#else
		#define TIMEY_API __declspec(dllimport)
	#endif
#else
	#define TIMEY_API
#endif

/**
  Obtains the current time and inserts it into a string buffer.

  @note For the buffer to hold the full time, the buffer must
  have a size of at least 11 bytes.
*/
TIMEY_API void timey_query_time(char *buffer, size_t buffer_size);
/**
  Obtains the current date and inserts it into a string buffer.

  @note For the buffer to hold the full date, the buffer must
  have a size of at least 13 bytes.
*/
TIMEY_API void timey_query_date(char *buffer, size_t buffer_size);

/**
  Obtains a timestamp with the current time.
*/
TIMEY_API timey_timestamp timey_curr_timestamp(void);
/**
  Obtains a timestamp in the future using another
  timestamp as a starting point.
*/
TIMEY_API timey_timestamp timey_future_timestamp(timey_timestamp *now, unsigned int hours, unsigned int minutes, unsigned int seconds);

/**
  Obtains a datetime with the current date and time.
*/
TIMEY_API timey_datetime timey_curr_datetime(void);
/**
  Obtains a datetime in the future using another
  datetime as a starting point.
*/
TIMEY_API timey_datetime timey_future_datetime(timey_datetime *now, unsigned int years, unsigned int months, unsigned int days, unsigned int hours, unsigned int minutes, unsigned int seconds);

/**
  Determines whether or not a year is a leap year.
*/
TIMEY_API bool timey_is_leap_year(unsigned int year);
/**
  Returns the number of days in a month.
*/
TIMEY_API unsigned int timey_num_days_in_month(unsigned int month);
/**
  Returns the number of days in the given date-time's
  month.
*/
TIMEY_API unsigned int timey_num_days_in_dt_month(timey_datetime *dt);
