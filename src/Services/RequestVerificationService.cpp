#include "Services/RequestVerificationService.hpp"

void RequestVerificationService::successAcceptanceVerification(uint8_t packetType,
                                                               bool secondaryHeaderFlag,
                                                               uint16_t APID, uint8_t seqFlag,
                                                               uint16_t packetSeqCount) {
	// TM[1,1] successful acceptance verification report
	assert(packetType <= 1);
	assert(APID <= 1024);
	assert(seqFlag <= 2);
	assert(packetSeqCount <= 8192);

	Message report = createTM(1);

	report.appendBits(3, ECSS_PUS_VERSION); //packet version number
	report.appendBits(1, packetType);
	report.appendBits(1, (uint16_t) secondaryHeaderFlag);
	report.appendBits(11, APID);
	report.appendBits(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	storeMessage(report);
}

void
RequestVerificationService::failAcceptanceVerification(uint8_t packetType,
                                                       bool secondaryHeaderFlag,
                                                       uint16_t APID, uint8_t seqFlag,
                                                       uint16_t packetSeqCount,
                                                       uint16_t code) {
	// TM[1,2] failed acceptance verification report
	assert(packetType <= 1);
	assert(APID <= 1024);
	assert(seqFlag <= 2);
	assert(packetSeqCount <= 8192);

	Message report = createTM(2);

	report.appendBits(3, ECSS_PUS_VERSION); //packet version number
	report.appendBits(1, packetType);
	report.appendBits(1, (uint16_t) secondaryHeaderFlag);
	report.appendBits(11, APID);
	report.appendBits(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	report.appendUint16(code);

	storeMessage(report);
}

void RequestVerificationService::successExecutionVerification(uint8_t packetType,
                                                              bool secondaryHeaderFlag,
                                                              uint16_t APID, uint8_t seqFlag,
                                                              uint16_t packetSeqCount) {
	// TM[1,7] successful completion of execution verification report
	assert(packetType <= 1);
	assert(APID <= 1024);
	assert(seqFlag <= 2);
	assert(packetSeqCount <= 8192);

	Message report = createTM(7);

	report.appendBits(3, ECSS_PUS_VERSION); //packet version number
	report.appendBits(1, packetType);
	report.appendBits(1, (uint16_t) secondaryHeaderFlag);
	report.appendBits(11, APID);
	report.appendBits(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	storeMessage(report);
}

void
RequestVerificationService::failExecutionVerification(uint8_t packetType,
                                                      bool secondaryHeaderFlag,
                                                      uint16_t APID, uint8_t seqFlag,
                                                      uint16_t packetSeqCount,
                                                      uint16_t code) {
	// TM[1,8] failed completion of execution verification report
	assert(packetType <= 1);
	assert(APID <= 1024);
	assert(seqFlag <= 2);
	assert(packetSeqCount <= 8192);

	Message report = createTM(8);

	report.appendBits(3, ECSS_PUS_VERSION); //packet version number
	report.appendBits(1, packetType);
	report.appendBits(1, (uint16_t) secondaryHeaderFlag);
	report.appendBits(11, APID);
	report.appendBits(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	report.appendUint16(code);

	storeMessage(report);
}

void
RequestVerificationService::failRoutingVerification(uint8_t packetType,
                                                    bool secondaryHeaderFlag,
                                                    uint16_t APID, uint8_t seqFlag,
                                                    uint16_t packetSeqCount,
                                                    uint16_t code) {
	// TM[1,10] failed routing verification report
	assert(packetType <= 1);
	assert(APID <= 1024);
	assert(seqFlag <= 2);
	assert(packetSeqCount <= 8192);

	Message report = createTM(10);

	report.appendBits(3, ECSS_PUS_VERSION); //packet version number
	report.appendBits(1, packetType);
	report.appendBits(1, (uint16_t) secondaryHeaderFlag);
	report.appendBits(11, APID);
	report.appendBits(2, seqFlag);
	report.appendBits(14, packetSeqCount);

	report.appendUint16(code);

	storeMessage(report);
}