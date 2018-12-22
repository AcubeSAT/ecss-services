#ifndef ECSS_SERVICES_MESSAGEPARSER_HPP
#define ECSS_SERVICES_MESSAGEPARSER_HPP

#include "Message.hpp"

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
	 * It is responsible to call the suitable function that executes the proper service. The way that
	 * the services are selected is based on the serviceType of the \p message
	 *
	 * @todo The implementation of the execute function should correspond to the numbers of the
	 * services/activities that have been created
	 */
	void execute(Message &message);

	/**
	 * Parse a message that contains the CCSDS and ECSS packet headers, as well as the data
	 *
	 * As defined in CCSDS 133.0-B-1
	 *
	 * @param data The data of the message (not null-terminated)
	 * @param length The size of the message
	 * @return A new object that represents the parsed message
	 */
	Message parse(uint8_t * data, uint32_t length);

private:
	/**
	 * Parse the ECSS Telecommand packet secondary header
	 *
	 * As specified in section 7.4.4.1 of the standard
	 *
	 * @todo Implement the acknowledgement flags
	 * @todo Store and parse the source ID, if needed
	 *
	 * @param data The data of the header (not null-terminated)
	 * @param length The size of the header
	 * @param message The Message to modify based on the header
	 */
	void parseTC(uint8_t *data, uint16_t length, Message &message);
};


#endif //ECSS_SERVICES_MESSAGEPARSER_HPP
