#ifndef ECSS_SERVICES_TESTSERVICE_HPP
#define ECSS_SERVICES_TESTSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[17] test service
 *
 * @ingroup Services
 */
class TestService : public Service {
public:
	inline static constexpr ServiceTypeNum ServiceType = 17;

	enum MessageType : uint8_t {
		AreYouAliveTest = 1,
		AreYouAliveTestReport = 2,
		OnBoardConnectionTest = 3,
		OnBoardConnectionTestReport = 4,
	};

	TestService() {
		serviceType = ServiceType;
	}

	/**
	 * TC[17,1] perform an are-you-alive connection test
	 */
	void areYouAlive(const Message& request);

	/**
	 * TM[17,2] are-you-alive connection test report to show that the MCU is alive and well
	 */
	void areYouAliveReport();

	/**
	 * TC[17,3] perform an on-board connection test
	 *
	 */
	void onBoardConnection(Message& request);

	/**
	 * TM[17,4] on-board connection test report to show that the MCU is connected to the on-board
	 */
	void onBoardConnectionReport(ApplicationProcessId applicationProcessId);

	/**
	 * It is responsible to call the suitable function that execute the proper subservice. The
	 * way that the subservices are selected is for the time being based on the messageType(class
	 * member of class Message) of the param message
	 *
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_TESTSERVICE_HPP
