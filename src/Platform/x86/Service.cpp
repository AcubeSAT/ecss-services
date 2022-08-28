#include <iostream>
#include <iomanip>
#include <Logger.hpp>
#include "Service.hpp"
#include <iostream>
#include <string>
#include <MessageParser.hpp>
#include <arpa/inet.h> // htons, inet_addr
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h> // uint16_t
#include <sys/socket.h> // socket, sendto
#include <unistd.h> // close

void Service::storeMessage(Message& message) {
	// appends the remaining bits to complete a byte
	message.finalize();

	// Create a new stream to display the packet
	std::ostringstream ss;

	// Just print it to the screen
	ss << "New " << ((message.packetType == Message::TM) ? "TM" : "TC") << "["
	   << std::hex
	   << static_cast<int>(message.serviceType) << "," // Ignore-MISRA
	   << static_cast<int>(message.messageType) // Ignore-MISRA
	   << "] message! ";

	for (unsigned int i = 0; i < message.dataSize; i++) {
		ss << static_cast<int>(message.data[i]) << " "; // Ignore-MISRA
	}


    // Send data to YAMCS port
	if(SendToYamcs) {
		std::string hostname{"127.0.0.1"};
		uint16_t port = 10015;

		int sock = ::socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in destination;
		destination.sin_family = AF_INET;
		destination.sin_port = htons(port);
		destination.sin_addr.s_addr = inet_addr(hostname.c_str());

		// Add ECSS and CCSDS header
		String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

		long bytesSent = ::sendto(sock, createdPacket.c_str(), createdPacket.length(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
		std::cout << bytesSent << " bytes sent" << std::endl;
		::close(sock);
	}
	LOG_DEBUG << ss.str();
}
