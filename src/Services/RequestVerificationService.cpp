#include "Services/RequestVerificationService.hpp"
#include "Message.hpp"

void RequestVerificationService::successAcceptanceVerification(Message &request) {
	// TM[1,1] successful acceptance verification report

	// parameters have max values defined from standard
	//assert(apid < 2048);
	//assert(seqFlag < 4);
	//assert(packetSeqCount < 16384);

	Message report = createTM(1);

	report.appendEnumerated(3, request.readEnumerated(3)); // packet version number
	report.appendEnumerated(1, request.readEnumerated(1)); // packet type
	report.appendBits(1, request.readBits(1)); // secondary header flag
	report.appendEnumerated(11, request.readEnumerated(11)); // application process ID
	report.appendEnumerated(2, request.readEnumerated(2)); // sequence flags
	report.appendBits(14, request.readBits(14)); // packet sequence count

	storeMessage(report);
}

void
RequestVerificationService::failAcceptanceVerification(Message &request) {
	// TM[1,2] failed acceptance verification report

	// parameters have max values defined from standard
	//assert(apid < 2048);
	//assert(seqFlag < 4);
	//assert(packetSeqCount < 16384);

	Message report = createTM(2);

	report.appendEnumerated(3, request.readEnumerated(3)); // packet version number
	report.appendEnumerated(1, request.readEnumerated(1)); // packet type
	report.appendBits(1, request.readBits(1)); // secondary header flag
	report.appendEnumerated(11, request.readEnumerated(11)); // application process ID
	report.appendEnumerated(2, request.readEnumerated(2)); // sequence flags
	report.appendBits(14, request.readBits(14)); // packet sequence count
	report.appendEnum16(request.readEnum16()); // error code

	storeMessage(report);
}

void RequestVerificationService::successExecutionVerification(Message &request) {
	// TM[1,7] successful completion of execution verification report

	// parameters have max values defined from standard
	//assert(apid < 2048);
	//assert(seqFlag < 4);
	//assert(packetSeqCount < 16384);

	Message report = createTM(7);

	report.appendEnumerated(3, request.readEnumerated(3)); // packet version number
	report.appendEnumerated(1, request.readEnumerated(1)); // packet type
	report.appendBits(1, request.readBits(1)); // secondary header flag
	report.appendEnumerated(11, request.readEnumerated(11)); // application process ID
	report.appendEnumerated(2, request.readEnumerated(2)); // sequence flags
	report.appendBits(14, request.readBits(14)); // packet sequence count

	storeMessage(report);
}

void
RequestVerificationService::failExecutionVerification(Message &request) {
	// TM[1,8] failed completion of execution verification report

	// parameters have max values defined from standard
	//assert(apid < 2048);
	//assert(seqFlag < 4);
	//assert(packetSeqCount < 16384);

	Message report = createTM(8);

	report.appendEnumerated(3, request.readEnumerated(3)); // packet version number
	report.appendEnumerated(1, request.readEnumerated(1)); // packet type
	report.appendBits(1, request.readBits(1)); // secondary header flag
	report.appendEnumerated(11, request.readEnumerated(11)); // application process ID
	report.appendEnumerated(2, request.readEnumerated(2)); // sequence flags
	report.appendBits(14, request.readBits(14)); // packet sequence count
	report.appendEnum16(request.readEnum16()); // error code

	storeMessage(report);
}

void
RequestVerificationService::failRoutingVerification(Message &request) {
	// TM[1,10] failed routing verification report

	// parameters have max values defined from standard
	//assert(apid < 2048);
	//assert(seqFlag < 4);
	//assert(packetSeqCount < 16384);

	Message report = createTM(10);

	report.appendEnumerated(3, request.readEnumerated(3)); // packet version number
	report.appendEnumerated(1, request.readEnumerated(1)); // packet type
	report.appendBits(1, request.readBits(1)); // secondary header flag
	report.appendEnumerated(11, request.readEnumerated(11)); // application process ID
	report.appendEnumerated(2, request.readEnumerated(2)); // sequence flags
	report.appendBits(14, request.readBits(14)); // packet sequence count
	report.appendEnum16(request.readEnum16()); // error code

	storeMessage(report);
}
