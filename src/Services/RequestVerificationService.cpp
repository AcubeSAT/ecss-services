#include "ECSS_Configuration.hpp"
#ifdef SERVICE_REQUESTVERIFICATION

#include "Services/RequestVerificationService.hpp"

void RequestVerificationService::successAcceptanceVerification(const ECSSMessage& request) {
	// TM[1,1] successful acceptance verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::SuccessfulAcceptanceReport);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failAcceptanceVerification(const ECSSMessage& request,
                                                            ErrorHandler::AcceptanceErrorType errorCode) {
	// TM[1,2] failed acceptance verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::FailedAcceptanceReport);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successStartExecutionVerification(const ECSSMessage& request) {
	// TM[1,3] successful start of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::SuccessfulStartOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failStartExecutionVerification(const ECSSMessage& request,
                                                                ErrorHandler::ExecutionStartErrorType errorCode) {
	// TM[1,4] failed start of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::FailedStartOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successProgressExecutionVerification(const ECSSMessage& request, uint8_t stepID) {
	// TM[1,5] successful progress of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::SuccessfulProgressOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendByte(stepID); // step ID

	storeMessage(report);
}

void RequestVerificationService::failProgressExecutionVerification(const ECSSMessage& request,
                                                                   ErrorHandler::ExecutionProgressErrorType errorCode,
                                                                   uint8_t stepID) {
	// TM[1,6] failed progress of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::FailedProgressOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendByte(stepID); // step ID
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::successCompletionExecutionVerification(const ECSSMessage& request) {
	// TM[1,7] successful completion of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::SuccessfulCompletionOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failCompletionExecutionVerification(
    const ECSSMessage& request, ErrorHandler::ExecutionCompletionErrorType errorCode) {
	// TM[1,8] failed completion of execution verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::FailedCompletionOfExecution);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

void RequestVerificationService::failRoutingVerification(const ECSSMessage& request,
                                                         ErrorHandler::RoutingErrorType errorCode) {
	// TM[1,10] failed routing verification report

	ECSSMessage report = createTM(RequestVerificationService::MessageType::FailedRoutingReport);

	report.appendEnumerated(3, CCSDSPacketVersion); // packet version number
	report.appendEnumerated(1, request.packetType); // packet type
	report.appendBits(1, 1); // secondary header flag
	report.appendEnumerated(11, request.applicationId); // application process ID
	report.appendEnumerated(2, ECSSSequenceFlags); // sequence flags
	report.appendBits(14, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode); // error code

	storeMessage(report);
}

#endif
