#include "Helpers/TimeFormats.hpp"
#include <iomanip>

bool Time::is_leap_year(uint16_t year) {
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

bool UTC_Timestamp::operator<(const UTC_Timestamp& Date) {
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

bool UTC_Timestamp::operator>(const UTC_Timestamp& Date) {
	if (this->year > Date.year) {
		return true;
	}
	if (this->year < Date.year) {
		return false;
	}
	if (this->month > Date.month) {
		return true;
	}
	if (this->month < Date.month) {
		return false;
	}
	if (this->day > Date.day) {
		return true;
	}
	if (this->day < Date.day) {
		return false;
	}
	if (this->hour > Date.hour) {
		return true;
	}
	if (this->hour < Date.hour) {
		return false;
	}
	if (this->minute > Date.minute) {
		return true;
	}
	if (this->minute < Date.minute) {
		return false;
	}
	if (this->second > Date.second) {
		return true;
	}

	return false;
}

bool UTC_Timestamp::operator==(const UTC_Timestamp& Date) {
	if (this->year != Date.year) {
		return false;
	}
	if (this->month != Date.month) {
		return false;
	}
	if (this->day != Date.day) {
		return false;
	}
	if (this->hour != Date.hour) {
		return false;
	}
	if (this->minute != Date.minute) {
		return false;
	}
	if (this->second != Date.second) {
		return false;
	}
	return true;
}

bool UTC_Timestamp::operator<=(const UTC_Timestamp& Date) {
	return ((*this < Date) || (*this == Date));
}

bool UTC_Timestamp::operator>=(const UTC_Timestamp& Date) {
	return ((*this > Date) || (*this == Date));
}

std::ostream& operator<<(std::ostream& o, UTC_Timestamp const& Date) { // NOLINT
	// YYYY-MM-DDTHH:mm:ss.sssZ
	o.fill('0');
	o << static_cast<int>(Date.year) << "-" << std::setw(2) << static_cast<int>(Date.month) << "-" << std::setw(2)
	  << static_cast<int>(Date.day) << "T" << std::setw(2) << static_cast<int>(Date.hour) << ":" << std::setw(2)
	  << static_cast<int>(Date.minute) << ":" << std::setw(2) << static_cast<int>(Date.second) << ":000Z" << std::endl;
	return o;
}
