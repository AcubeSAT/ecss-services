#include <Time/UTCTimestamp.hpp>
#include <ostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& o, UTCTimestamp const& date) {
	// YYYY-MM-DDTHH:mm:ss.sssZ
	o.fill('0');
	o << static_cast<int>(date.year) << "-" << std::setw(2) << static_cast<int>(date.month) << "-" << std::setw(2)
	  << static_cast<int>(date.day) << "T" << std::setw(2) << static_cast<int>(date.hour) << ":" << std::setw(2)
	  << static_cast<int>(date.minute) << ":" << std::setw(2) << static_cast<int>(date.second) << ":000Z" << std::endl;
	return o;
}
