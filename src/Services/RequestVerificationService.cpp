#include "Services/RequestVerificationService.hpp"
#include "Message.hpp"

void RequestVerificationService::successAcceptanceVerification(const Message &request) {
	// TM[1,1] successful acceptance verification report

	Message report = createTM(1);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, 0); // sequence flags(not implemented)
	report.appendBits(14, 0); // packet sequence count(not implemented)

	storeMessage(report);
}

void
RequestVerificationService::failAcceptanceVerification(const Message &request) {
	// TM[1,2] failed acceptance verification report

	Message report = createTM(2);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, 0); // sequence flags(not implemented)
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(0); // error code(not implemented)

	storeMessage(report);
}

void RequestVerificationService::successExecutionVerification(const Message &request) {
	// TM[1,7] successful completion of execution verification report

	Message report = createTM(7);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, 0); // sequence flags(not implemented)
	report.appendBits(14, 0); // packet sequence count(not implemented)

	storeMessage(report);
}

void
RequestVerificationService::failExecutionVerification(const Message &request) {
	// TM[1,8] failed completion of execution verification report

	Message report = createTM(8);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, 0); // sequence flags(not implemented)
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(0); // error code(not implemented)

	storeMessage(report);
}

void
RequestVerificationService::failRoutingVerification(const Message &request) {
	// TM[1,10] failed routing verification report

	Message report = createTM(10);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, 0); // sequence flags(not implemented)
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(0); // error code(not implemented)

	storeMessage(report);
}

void RequestVerificationService::execute(const Message &message) {
	switch (message.messageType) {
		case 1:
			successAcceptanceVerification(message);
			break;
		case 2:
			failAcceptanceVerification(message);
			break;
		case 7:
			successExecutionVerification(message);
			break;
		case 8:
			failExecutionVerification(message);
			break;
		case 10:
			failRoutingVerification(message);
			break;
		default:
			// cout is very bad for embedded systems
			std::cout << "Error: There is not such a message type in ST[01] service";
			assert(false);
			break;
	}
}
