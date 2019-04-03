#ifndef ECSS_SERVICES_TIMEGETTER_HPP
#define ECSS_SERVICES_TIMEGETTER_HPP

#include <iostream>
#include <cstdint>
#include <ctime>


/**
 * @brief Get the current time
 */
class TimeGetter {
public:

	/**
	 * @brief Gets the current time in UNIX epoch
	 * @return Current UNIX epoch time, in elapsed seconds
	 */
	static inline uint32_t getSeconds() {
		return static_cast<uint32_t >(time(nullptr));
	}
};

#endif //ECSS_SERVICES_TIMEGETTER_HPP
