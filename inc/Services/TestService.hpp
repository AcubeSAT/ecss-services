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
	void areYouAlive(const Message & request);

	/**
	 * TC[17,3] perform an on-board connection test
	 *
	 * @todo Only respond if we have the correct APID
	 */
	void onBoardConnection(const Message & request);
};


#endif //ECSS_SERVICES_TESTSERVICE_HPP
