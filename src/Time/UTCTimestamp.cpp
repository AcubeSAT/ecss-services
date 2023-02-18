#include "Time/Time.hpp"
#include "Time/UTCTimestamp.hpp"
#include <iomanip>

UTCTimestamp::UTCTimestamp() : year(1970), month(1), second(0), minute(0), hour(0), day(1) {}

UTCTimestamp::UTCTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	ASSERT_INTERNAL(1970 <= year, ErrorHandler::InternalErrorType::InvalidDate);
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

bool UTCTimestamp::operator<(const UTCTimestamp& Date) const {
	if (this->year < Date.year) {
		return true;
	}
	if (this->year > Date.year) {
		return false;
	}

	if (this->month < Date.month) {
		return true;
	}
	if (this->month > Date.month) {
		return false;
	}

	if (this->day < Date.day) {
		return true;
	}
	if (this->day > Date.day) {
		return false;
	}

	if (this->hour < Date.hour) {
		return true;
	}
	if (this->hour > Date.hour) {
		return false;
	}

	if (this->minute < Date.minute) {
		return true;
	}
	if (this->minute > Date.minute) {
		return false;
	}

	if (this->second < Date.second) {
		return true;
	}

	return false;
}

bool UTCTimestamp::operator>(const UTCTimestamp& Date) const {
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

bool UTCTimestamp::operator==(const UTCTimestamp& Date) const {
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

bool UTCTimestamp::operator<=(const UTCTimestamp& Date) const {
	return ((*this < Date) || (*this == Date));
}

bool UTCTimestamp::operator>=(const UTCTimestamp& Date) const {
	return ((*this > Date) || (*this == Date));
}
void UTCTimestamp::repair() {
	using namespace Time;

	if (second > Time::SecondsPerMinute) {
		second -= Time::SecondsPerMinute;
		minute++;
	}

	const auto MinutesPerHour = SecondsPerHour / SecondsPerMinute;
	if (minute >= MinutesPerHour) {
		minute -= MinutesPerHour;
		hour++;
	}

	const auto HoursPerDay = SecondsPerDay / SecondsPerHour;
	if (hour >= HoursPerDay) {
		hour -= HoursPerDay;
		day++;
	}

	if (day > daysOfMonth()) {
		day -= daysOfMonth();
		month++;
	}

	if (month > MonthsPerYear) {
		month -= MonthsPerYear;
		year++;
	}
}
