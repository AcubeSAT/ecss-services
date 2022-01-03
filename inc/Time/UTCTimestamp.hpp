#pragma once

#include <cstdint>
#include <etl/String.hpp>

/**
* A class that represents a UTC time and date according to ISO 8601
*
* This class contains a human-readable representation of a timestamp, accurate down to 1 second. It is not used
* for timestamp storage in the satellite due to its high performance and memory cost, but it can be used for
* debugging and logging purposes.
*
* @note
* This class represents UTC (Coordinated Universal Time) date
*/
class UTCTimestamp {
public:
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t hour;
   uint8_t minute;
   uint8_t second;

   /**
	* Initialise a timestamp with the Unix epoch 1/1/1970 00:00:00
	*/
   UTCTimestamp();

   /**
	*
	* @todo See if this implements leap seconds
	* @todo Implement leap seconds as ST[20] parameter
	* @param year the year as it used in Gregorian calendar
	* @param month the month as it used in Gregorian calendar (1-12 inclusive)
	* @param day the day as it used in Gregorian calendar (1-31 inclusive)
	* @param hour UTC hour in 24-hour format
	* @param minute UTC minutes
	* @param second UTC seconds
	*/
   UTCTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

   /**
	* @param textTimestamp the timestamp to parse into a UTC date
	* @todo Too expensive to implement (?). It is better to remove this and open it as another issue, or create
	* a platform-specific converter that will be only used in x86.
	*/
   explicit UTCTimestamp(etl::string<32> textTimestamp);

   /**
	* Compare two timestamps.
	* @param Date the date that will be compared with the pointer `this`
	*/
   bool operator<(const UTCTimestamp& Date) const;
   bool operator>(const UTCTimestamp& Date) const; ///< @copydoc UTCTimestamp::operator<
   bool operator==(const UTCTimestamp& Date) const; ///< @copydoc UTCTimestamp::operator<
   bool operator<=(const UTCTimestamp& Date) const; ///< @copydoc UTCTimestamp::operator<
   bool operator>=(const UTCTimestamp& Date) const; ///< @copydoc UTCTimestamp::operator<
};

