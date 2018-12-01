#ifndef ECSS_SERVICES_MESSAGEPARSER_HPP
#define ECSS_SERVICES_MESSAGEPARSER_HPP

// include all the header files for the services that have been implemented for the time being
#include "Services/TestService.hpp"
#include "Services/RequestVerificationService.hpp"
#include <iostream> // This file should be removed

/**
 * A generic class responsible for the execution of the incoming telemetry and telecommand
 * packets.
 *
 * @todo Make the connection between the ST[01]-request verification service and the services
 * that initiate it
 * @todo Implement the execute function in the upcoming services or generally in the upcoming
 * activities
 *
 */

class MessageParser {
public:

/**
 * It is responsible to call the suitable function that execute the proper service. The way that
 * the services are selected is based on the serviceType(class member of Message) of the param
 * message
 *
 * @todo The implementation of the execute function should correspond to the numbers of the
 * services/activities that have been created
 */
	void execute(Message &message);
};


#endif //ECSS_SERVICES_MESSAGEPARSER_HPP
