#include "Service.hpp"
#include <Logger.hpp>
#include <MessageParser.hpp>
#include <arpa/inet.h>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class PacketSender {
private:
	const char* hostname = "127.0.0.1";
	const uint16_t port = 10015;
	sockaddr_in destination;
	int socket;

public:
	PacketSender() {
		socket = ::socket(AF_INET, SOCK_DGRAM, 0);
		destination.sin_family = AF_INET;
		destination.sin_port = htons(port);
		destination.sin_addr.s_addr = inet_addr(hostname);
	};

	~PacketSender() {
		::close(socket);
	};

	void sendPacketToYamcs(Message& message) {
		// Add ECSS and CCSDS header
		String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
		auto bytesSent = ::sendto(socket, createdPacket.c_str(), createdPacket.length(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
		LOG_DEBUG << bytesSent << " bytes sent";
	}
};

PacketSender packetSender;

/**
 * If set to true, the created messages will be sent to port 10025 on localhost for testing purposes.
 */
inline const bool SendToYamcs = true;

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
	if constexpr (SendToYamcs) {
		packetSender.sendPacketToYamcs(message);
	}
	LOG_DEBUG << ss.str();
}
