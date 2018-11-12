#ifndef ECSS_SERVICES_TESTSERVICE_HPP
#define ECSS_SERVICES_TESTSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[17] test service
 */
class TestService : public Service {
public:
	TestService() {
		serviceType = 17;
	}

	/**
	 * TC[17,1] perform an are-you-alive connection test
	 */
	void areYouAlive(const Message & message);
};


#endif //ECSS_SERVICES_TESTSERVICE_HPP
