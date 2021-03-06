#include "Helpers/TimeHelper.hpp"

TimeAndDate::TimeAndDate() {
	// Unix epoch 1/1/1970
	year = 1970;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	second = 0;
}

TimeAndDate::TimeAndDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
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

bool TimeAndDate::operator<(const TimeAndDate& Date) {
	// compare years
	if (this->year < Date.year) {
		return true;
	}
	if (this->year > Date.year) {
		return false;
	}

	// compare months
	if (this->month < Date.month) {
		return true;
	}
	if (this->month > Date.month) {
		return false;
	}

	// compare days
	if (this->day < Date.day) {
		return true;
	}
	if (this->day > Date.day) {
		return false;
	}

	// compare hours
	if (this->hour < Date.hour) {
		return true;
	}
	if (this->hour > Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute < Date.minute) {
		return true;
	}
	if (this->minute > Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second < Date.second) {
		return true;
	}

	return false;
}

bool TimeAndDate::operator>(const TimeAndDate& Date) {
	// compare years
	if (this->year > Date.year) {
		return true;
	}
	if (this->year < Date.year) {
		return false;
	}

	// compare months
	if (this->month > Date.month) {
		return true;
	}
	if (this->month < Date.month) {
		return false;
	}

	// compare days
	if (this->day > Date.day) {
		return true;
	}
	if (this->day < Date.day) {
		return false;
	}

	// compare hours
	if (this->hour > Date.hour) {
		return true;
	}
	if (this->hour < Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute > Date.minute) {
		return true;
	}
	if (this->minute < Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second > Date.second) {
		return true;
	}

	return false;
}

bool TimeAndDate::operator==(const TimeAndDate& Date) {
	// compare years
	if (this->year != Date.year) {
		return false;
	}

	// compare months
	if (this->month != Date.month) {
		return false;
	}

	// compare days
	if (this->day != Date.day) {
		return false;
	}

	// compare hours
	if (this->hour != Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute != Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second != Date.second) {
		return false;
	}

	return true;
}

bool TimeAndDate::operator<=(const TimeAndDate& Date) {
	return ((*this < Date) || (*this == Date));
}

bool TimeAndDate::operator>=(const TimeAndDate& Date) {
	return ((*this > Date) || (*this == Date));
}
