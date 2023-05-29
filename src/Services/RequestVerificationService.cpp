#include "ECSS_Configuration.hpp"
#ifdef SERVICE_REQUESTVERIFICATION

#include "Services/RequestVerificationService.hpp"

const uint8_t CCSDSPacketVersionBits = 3;  //enumerated (3 bits)
const uint8_t packetTypeBits = 1;  //enumerated (1 bit)
const uint8_t secondaryHeaderFlagBits = 1 ;  //Boolean (1bit)
const uint8_t applicationIdBits = 11; //enumerated (11 bits)
const uint8_t ECSSSequenceFlagsBits = 2;  //enumerated (2 bits)
const uint8_t packetSequenceCountBits = 14; //unsigned integer (14 bits)


    void RequestVerificationService::successAcceptanceVerification(const Message& request) {
	// TM[1,1] successful acceptance verification report

	Message report = createTM(RequestVerificationService::MessageType::SuccessfulAcceptanceReport);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);  // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);                // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);  // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failAcceptanceVerification(const Message& request,
                                                            ErrorHandler::AcceptanceErrorType errorCode) {
	// TM[1,2] failed acceptance verification report

	Message report = createTM(RequestVerificationService::MessageType::FailedAcceptanceReport);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);              // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);    // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode);                     // error code

	storeMessage(report);
}

void RequestVerificationService::successStartExecutionVerification(const Message& request) {
	// TM[1,3] successful start of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::SuccessfulStartOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);               // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failStartExecutionVerification(const Message& request,
                                                                ErrorHandler::ExecutionStartErrorType errorCode) {
	// TM[1,4] failed start of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::FailedStartOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);               // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode);                     // error code

	storeMessage(report);
}

void RequestVerificationService::successProgressExecutionVerification(const Message& request, uint8_t stepID) {
	// TM[1,5] successful progress of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::SuccessfulProgressOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);               // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendByte(stepID);                          // step ID

	storeMessage(report);
}

void RequestVerificationService::failProgressExecutionVerification(const Message& request,
                                                                   ErrorHandler::ExecutionProgressErrorType errorCode,
                                                                   uint8_t stepID) {
	// TM[1,6] failed progress of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::FailedProgressOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);                // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendByte(stepID);                          // step ID
	report.appendEnum16(errorCode);                     // error code

	storeMessage(report);
}

void RequestVerificationService::successCompletionExecutionVerification(const Message& request) {
	// TM[1,7] successful completion of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::SuccessfulCompletionOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);                // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count

	storeMessage(report);
}

void RequestVerificationService::failCompletionExecutionVerification(
    const Message& request, ErrorHandler::ExecutionCompletionErrorType errorCode) {
	// TM[1,8] failed completion of execution verification report

	Message report = createTM(RequestVerificationService::MessageType::FailedCompletionOfExecution);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);               // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode);                     // error code

	storeMessage(report);
}

void RequestVerificationService::failRoutingVerification(const Message& request,
                                                         ErrorHandler::RoutingErrorType errorCode) {
	// TM[1,10] failed routing verification report

	Message report = createTM(RequestVerificationService::MessageType::FailedRoutingReport);

	report.appendEnumerated(CCSDSPacketVersionBits, CCSDSPacketVersion);     // packet version number
	report.appendEnumerated(packetTypeBits, request.packetType);     // packet type
	report.appendBits(secondaryHeaderFlagBits, 1);                // secondary header flag
	report.appendEnumerated(applicationIdBits, request.applicationId); // application process ID
	report.appendEnumerated(ECSSSequenceFlagsBits, ECSSSequenceFlags);      // sequence flags
	report.appendBits(packetSequenceCountBits, request.packetSequenceCount); // packet sequence count
	report.appendEnum16(errorCode);                     // error code

	storeMessage(report);
}

#endif
