#include "Helpers/TimeHelper.hpp"

bool TimeAndDate::operator<(const TimeAndDate &Date) {
	//@note clang tidy complaints when using else after return so that's why there are these
	// numerous if checks. Sorry for the ugly code...

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
	//@note clang tidy complaints when using else after return so that's why there are these
	// numerous if checks. Sorry for the ugly code...

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

bool TimeAndDate::operator<=(const TimeAndDate &Date) {
	//@note clang tidy complaints when using else after return so that's why there are these
	// numerous if checks. Sorry for the ugly code...

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
	if (this->second > Date.second) {
		return false;
	}

	return true;

}

bool TimeAndDate::operator>=(const TimeAndDate &Date) {
	//@note clang tidy complaints when using else after return so that's why there are these
	// numerous if checks. Sorry for the ugly code...

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
	if (this->second < Date.second) {
		return false;
	}

	return true;

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
