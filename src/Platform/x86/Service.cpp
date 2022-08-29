#include "Service.hpp"
#include <Logger.hpp>
#include <iomanip>
#include <iostream>
#include <MessageParser.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void Service::storeMessage(Message& message) {
	// appends the remaining bits to complete a byte
	message.finalize();

	// Create a new stream to display the packet
	std::ostringstream ss;

	// Just print it to the screen
	ss << "New " << ((message.packetType == Message::TM) ? "TM" : "TC") << "["
	   << std::hex
	   << static_cast<int>(message.serviceType) << "," // Ignore-MISRA
	   << static_cast<int>(message.messageType)        // Ignore-MISRA
	   << "] message! ";

	for (unsigned int i = 0; i < message.dataSize; i++) {
		ss << static_cast<int>(message.data[i]) << " "; // Ignore-MISRA
	}


	// Send data to YAMCS port
	if (SendToYamcs) {
		const char* hostname = "127.0.0.1";
		uint16_t port = 10015;

		int sock = ::socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in destination;
		destination.sin_family = AF_INET;
		destination.sin_port = htons(port);
		destination.sin_addr.s_addr = inet_addr(hostname);

		// Add ECSS and CCSDS header
		String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

		auto bytesSent = ::sendto(sock, createdPacket.c_str(), createdPacket.length(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
		LOG_DEBUG << bytesSent << " bytes sent";
		::close(sock);
	}
	LOG_DEBUG << ss.str();
}
