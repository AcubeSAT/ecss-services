#include <iostream>
#include <iomanip>
#include "Service.hpp"

void Service::storeMessage(const Message &message) {
	// Just print it to the screen
	std::cout << "New " << ((message.packetType == Message::TM) ? "TM" : "TC") << "[" << std::dec
	          << static_cast<int>(message.serviceType) << ","
	          << static_cast<int>(message.messageType) << "] message!\n";
	//std::cout << std::hex << std::setfill('0') << std::setw(2);
	for (int i = 0; i < message.dataSize; i++) {
		std::cout << static_cast<int>(message.data[i]);
		std::cout << " ";
	}
	std::cout << std::endl;
}
