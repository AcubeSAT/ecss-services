#ifndef ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * Note:For the time being, the cause(routing, acceptance, execution), that functions of ST[01]
 * should be called, hasn't been implemented yet. In main.cpp there are some random calls with
 * dummy values.
 *
 * @todo All telemetry packets shall have a telemetry packet secondary header
 * @todo See if the deduced data defined from the standard should still be ignored. This deduced
 * data exists only in reports that send failure signs(for example the TM[1,2])
 */
class RequestVerificationService : public Service {
public:
	RequestVerificationService() {
		serviceType = 1;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 *
	 * Take a Message& reference as argument, that contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that accepted successfully
	 *
	 */
	void successAcceptanceVerification(Message &request);

	/**
	 * TM[1,2] failed acceptance verification report
	 *
	 * Take a Message& reference as argument, that contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed to be accepted
	 *
	 */
	void failAcceptanceVerification(Message &request);


	/**
 	 * TM[1,7] successful completion of execution verification report
	 *
	 * Take a Message& reference as argument, that contains the necessary data to send the report.
	 * The data is actually data members of Message that contain the basic info of the
	 * telecommand packet that executed successfully
	 *
 	 */
	void successExecutionVerification(Message &request);

	/**
	 * TM[1,8] failed completion of execution verification report
	 *
	 * Take a Message& reference as argument, that contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed  to be executed
	 *
	 */
	void failExecutionVerification(Message &request);

	/**
	 * TM[1,10] failed routing verification report
	 *
	 * Take a Message& reference as argument, that contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed the routing
	 *
 	 */
	void failRoutingVerification(Message &request);


};

#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
