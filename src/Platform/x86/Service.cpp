#include <iostream>
#include <iomanip>
#include <Logger.hpp>
#include "Service.hpp"

void Service::storeMessage(Message& message) {
	// appends the remaining bits to complete a byte
	message.finalize();

	// Create a new stream to display the packet
	std::ostringstream ss;

	// Just print it to the screen
	ss << "New " << ((message.packetType == Message::TM) ? "TM" : "TC") << "["
	   << std::hex
	   << static_cast<int>(message.serviceType) << "," << static_cast<int>(message.messageType)
	   << "] message! ";

	for (unsigned int i = 0; i < message.dataSize; i++) {
		ss << static_cast<int>(message.data[i]) << " "; // Ignore-MISRA
	}

	LOG_DEBUG << ss.str();
}
