#include "Services/RequestVerificationService.hpp"
#include "Message.hpp"

void RequestVerificationService::successAcceptanceVerification(Message::PacketType packetType,
                                                               bool secondaryHeaderFlag,
                                                               uint16_t apid, uint8_t seqFlag,
                                                               uint16_t packetSeqCount) {
	// TM[1,1] successful acceptance verification report
	assert(apid < 2048);
	assert(seqFlag < 4);
	assert(packetSeqCount < 16384);

	Message report = createTM(1);

	report.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	report.appendEnumerated(1, packetType);
	report.appendBits(1, static_cast<uint16_t >(secondaryHeaderFlag));
	report.appendEnumerated(11, apid);
	report.appendEnumerated(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	storeMessage(report);
}

void
RequestVerificationService::failAcceptanceVerification(Message::PacketType packetType,
                                                       bool secondaryHeaderFlag,
                                                       uint16_t apid, uint8_t seqFlag,
                                                       uint16_t packetSeqCount,
                                                       uint16_t errorCode) {
	// TM[1,2] failed acceptance verification report
	assert(apid < 2048);
	assert(seqFlag < 4);
	assert(packetSeqCount < 16384);

	Message report = createTM(2);

	report.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	report.appendEnumerated(1, packetType);
	report.appendBits(1, static_cast<uint16_t >(secondaryHeaderFlag));
	report.appendEnumerated(11, apid);
	report.appendEnumerated(2, seqFlag);
	report.appendBits(14, packetSeqCount);
	report.appendUint16(errorCode);

	storeMessage(report);
}

void RequestVerificationService::successExecutionVerification(Message::PacketType packetType,
                                                              bool secondaryHeaderFlag,
                                                              uint16_t apid, uint8_t seqFlag,
                                                              uint16_t packetSeqCount) {
	// TM[1,7] successful completion of execution verification report
	assert(apid < 2048);
	assert(seqFlag < 4);
	assert(packetSeqCount < 16384);

	Message report = createTM(7);

	report.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	report.appendEnumerated(1, packetType);
	report.appendBits(1, static_cast<uint16_t >(secondaryHeaderFlag));
	report.appendEnumerated(11, apid);
	report.appendEnumerated(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	storeMessage(report);
}

void
RequestVerificationService::failExecutionVerification(Message::PacketType packetType,
                                                      bool secondaryHeaderFlag,
                                                      uint16_t apid, uint8_t seqFlag,
                                                      uint16_t packetSeqCount,
                                                      uint16_t errorCode) {
	// TM[1,8] failed completion of execution verification report
	assert(apid < 2048);
	assert(seqFlag < 4);
	assert(packetSeqCount < 16384);

	Message report = createTM(8);

	report.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	report.appendEnumerated(1, packetType);
	report.appendBits(1, static_cast<uint16_t >(secondaryHeaderFlag));
	report.appendEnumerated(11, apid);
	report.appendEnumerated(2, seqFlag);
	report.appendBits(14, packetSeqCount);
	report.appendUint16(errorCode);

	storeMessage(report);
}

void
RequestVerificationService::failRoutingVerification(Message::PacketType packetType,
                                                    bool secondaryHeaderFlag,
                                                    uint16_t apid, uint8_t seqFlag,
                                                    uint16_t packetSeqCount,
                                                    uint16_t errorCode) {
	// TM[1,10] failed routing verification report
	assert(apid < 2048);
	assert(seqFlag < 4);
	assert(packetSeqCount < 16384);

	Message report = createTM(10);

	report.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	report.appendEnumerated(1, packetType);
	report.appendBits(1, static_cast<uint16_t >(secondaryHeaderFlag));
	report.appendEnumerated(11, apid);
	report.appendEnumerated(2, seqFlag);
	report.appendBits(14, packetSeqCount);
	report.appendUint16(errorCode);

	storeMessage(report);
}
