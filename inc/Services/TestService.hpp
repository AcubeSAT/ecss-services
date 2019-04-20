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
	void areYouAlive(Message& request);

	/**
	 * TC[17,3] perform an on-board connection test
	 *
	 * @todo Only respond if we have the correct APID
	 */
	void onBoardConnection(Message& request);

	/**
	 * It is responsible to call the suitable function that execute the proper subservice. The
	 * way that the subservices are selected is for the time being based on the messageType(class
	 * member of class Message) of the param message
	 *
	 * @todo Error handling for the switch() in the implementation of this execute function
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_TESTSERVICE_HPP
