#include "Services/RequestVerificationService.hpp"


void RequestVerificationService::successAcceptanceVerification(const Message &request) {
	// TM[1,1] successful acceptance verification report

	Message report = createTM(1);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)

	storeMessage(report);
}

void
RequestVerificationService::failAcceptanceVerification(const Message &request,
	ErrorHandler::AcceptanceErrorType errorCode) {
	// TM[1,2] failed acceptance verification report

	Message report = createTM(2);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successStartExecutionVerification(const Message &request) {
	// TM[1,3] successful start of execution verification report

	Message report = createTM(3);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failStartExecutionVerification(const Message &request,
	ErrorHandler::ExecutionStartErrorType errorCode) {
	// TM[1,4] failed start of execution verification report

	Message report = createTM(4);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successProgressExecutionVerification(const Message &request,
	uint8_t stepID) {
	// TM[1,5] successful progress of execution verification report

	Message report = createTM(5);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendByte(stepID); // step ID

	storeMessage(report);
}

void RequestVerificationService::failProgressExecutionVerification(const Message &request,
	ErrorHandler::ExecutionProgressErrorType errorCode, uint8_t stepID) {
	// TM[1,6] failed progress of execution verification report

	Message report = createTM(6);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendByte(stepID); // step ID
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successCompletionExecutionVerification(const Message &request) {
	// TM[1,7] successful completion of execution verification report

	Message report = createTM(7);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)

	storeMessage(report);
}

void
RequestVerificationService::failCompletionExecutionVerification(const Message &request,
	ErrorHandler::ExecutionCompletionErrorType errorCode) {
	// TM[1,8] failed completion of execution verification report

	Message report = createTM(8);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void
RequestVerificationService::failRoutingVerification(const Message &request,
	ErrorHandler::RoutingErrorType errorCode) {
	// TM[1,10] failed routing verification report

	Message report = createTM(10);

	report.appendEnumerated(3, CCSDS_PACKET_VERSION); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 0); // secondary header flag(not implemented)
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSS_SEQUENCE_FLAGS); // sequence flags
	report.appendBits(14, 0); // packet sequence count(not implemented)
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}
