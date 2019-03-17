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

TimeAndDate::TimeAndDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute,
                         uint8_t second) {
	// check if the parameters makes sense

	// @todo change the condition of the assertion for the \p year
	assertI(2019 <= year < 2025, ErrorHandler::InternalErrorType::UnknownInternalError);
	assertI(1 <= month <= 12, ErrorHandler::InternalErrorType::UnknownInternalError);
	assertI(1 <= day <= 31, ErrorHandler::InternalErrorType::UnknownInternalError);
	assertI(0 <= hour <= 24, ErrorHandler::InternalErrorType::UnknownInternalError);
	assertI(0 <= minute <= 60, ErrorHandler::InternalErrorType::UnknownInternalError);
	assertI(0 <= second <= 60, ErrorHandler::InternalErrorType::UnknownInternalError);

	this->year = year;
	this->month = month;
	this->hour = hour;
	this->day = day;
	this->minute = minute;
	this->second = second;
}

bool TimeAndDate::operator<(const TimeAndDate &Date) {
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

bool TimeAndDate::operator>(const TimeAndDate &Date) {
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

bool TimeAndDate::operator==(const TimeAndDate &Date) {
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


bool TimeAndDate::operator<=(const TimeAndDate &Date) {
	return (*this < Date || *this == Date);
}

bool TimeAndDate::operator>=(const TimeAndDate &Date) {
	return (*this > Date || *this == Date);
}

