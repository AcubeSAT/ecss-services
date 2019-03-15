#include "Helpers/TimeHelper.hpp"

bool TimeAndDate::operator<(const TimeAndDate &Date) {
	// compare years
	if (this->year < Date.year) {
		return true;
	} else if (this->year > Date.year) {
		return false;
	}

	// compare months
	if (this->month < Date.month) {
		return true;
	} else if (this->month > Date.month) {
		return false;
	}

	// compare days
	if (this->day < Date.day) {
		return true;
	} else if (this->day > Date.day) {
		return false;
	}

	// compare hours
	if (this->hour < Date.hour) {
		return true;
	} else if (this->hour > Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute < Date.minute) {
		return true;
	} else if (this->minute > Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second < Date.second) {
		return true;
	} else {
		return false;
	}
}

bool TimeAndDate::operator>(const TimeAndDate &Date) {
	// compare years
	if (this->year > Date.year) {
		return true;
	} else if (this->year < Date.year) {
		return false;
	}

	// compare months
	if (this->month > Date.month) {
		return true;
	} else if (this->month < Date.month) {
		return false;
	}

	// compare days
	if (this->day > Date.day) {
		return true;
	} else if (this->day < Date.day) {
		return false;
	}

	// compare hours
	if (this->hour > Date.hour) {
		return true;
	} else if (this->hour < Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute > Date.minute) {
		return true;
	} else if (this->minute < Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second > Date.second) {
		return true;
	} else {
		return false;
	}
}

bool TimeAndDate::operator<=(const TimeAndDate &Date) {
	// compare years
	if (this->year < Date.year) {
		return true;
	} else if (this->year > Date.year) {
		return false;
	}

	// compare months
	if (this->month < Date.month) {
		return true;
	} else if (this->month > Date.month) {
		return false;
	}

	// compare days
	if (this->day < Date.day) {
		return true;
	} else if (this->day > Date.day) {
		return false;
	}

	// compare hours
	if (this->hour < Date.hour) {
		return true;
	} else if (this->hour > Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute < Date.minute) {
		return true;
	} else if (this->minute > Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second < Date.second) {
		return true;
	} else if (this->second > Date.second) {
		return false;
	} else {
		return true;
	}
}

bool TimeAndDate::operator>=(const TimeAndDate &Date) {
	// compare years
	if (this->year > Date.year) {
		return true;
	} else if (this->year < Date.year) {
		return false;
	}

	// compare months
	if (this->month > Date.month) {
		return true;
	} else if (this->month < Date.month) {
		return false;
	}

	// compare days
	if (this->day > Date.day) {
		return true;
	} else if (this->day < Date.day) {
		return false;
	}

	// compare hours
	if (this->hour > Date.hour) {
		return true;
	} else if (this->hour < Date.hour) {
		return false;
	}

	// compare minutes
	if (this->minute > Date.minute) {
		return true;
	} else if (this->minute < Date.minute) {
		return false;
	}

	// compare seconds
	if (this->second > Date.second) {
		return true;
	} else if (this->second < Date.second) {
		return false;
	} else {
		return true;
	}
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
	} else {
		return true;
	}
}