#include "Helpers/TimeFormats.hpp"

bool is_leap_year(uint16_t year) {
	if ((year % 4) != 0) {
		return false;
	}
	if ((year % 100) != 0) {
		return true;
	}
	return (year % 400) == 0;
}

UTC_Timestamp::UTC_Timestamp() {
	// Unix epoch 1/1/1970
	year = 1970;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	second = 0;
}

UTC_Timestamp::UTC_Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	// check if the parameters make sense
	ASSERT_INTERNAL(2019 <= year, ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL((1 <= month) && (month <= 12), ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL((1 <= day) && (day <= 31), ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL(hour < 24, ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL(minute < 60, ErrorHandler::InternalErrorType::InvalidDate);
	// Seconds can be equal to 60, to account for leap seconds.
	ASSERT_INTERNAL(second <= 60, ErrorHandler::InternalErrorType::InvalidDate);

	this->year = year;
	this->month = month;
	this->hour = hour;
	this->day = day;
	this->minute = minute;
	this->second = second;
}
