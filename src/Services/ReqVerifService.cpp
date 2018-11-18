#include "Services/ReqVerifService.hpp"

void ReqVerifService::successAcceptVerif(uint8_t packetVersionNum, uint8_t packetType,
                                         bool secondaryHeaderFlag, uint16_t APID, uint8_t seqFlag,
                                         uint16_t packetSeqCount) {
	// TM[1,1] successful acceptance verification report
	assert(packetVersionNum <= 4 & packetType <= 1 & APID <= 1024 & seqFlag <= 2 &
	       packetSeqCount <= 8192);

	uint32_t value;

	value = (packetVersionNum << 29 | packetType << 28 | secondaryHeaderFlag << 27 | APID << 16 |
	         seqFlag << 14 | packetSeqCount);

	Message report = createTM(1);

	report.appendUint32(value);

	storeMessage(report);
}

void ReqVerifService::failAccessVerif(uint8_t packetVersionNum, uint8_t packetType,
                                      bool secondaryHeaderFlag,
                                      uint16_t APID, uint8_t seqFlag, uint16_t packetSeqCount,
                                      uint16_t code) {
	// TM[1,2] failed acceptance verification report
	assert(packetVersionNum <= 4 & packetType <= 1 & APID <= 1024 & seqFlag <= 2 &
	       packetSeqCount <= 8192);

	uint32_t value;

	value = (packetVersionNum << 29 | packetType << 28 | secondaryHeaderFlag << 27 | APID << 16 |
	         seqFlag << 14 | packetSeqCount);

	Message report = createTM(2);

	report.appendUint32(value);

	report.appendUint16(code);

	storeMessage(report);
}

void ReqVerifService::successExeVerif(uint8_t packetVersionNum, uint8_t packetType,
                                      bool secondaryHeaderFlag, uint16_t APID, uint8_t seqFlag,
                                      uint16_t packetSeqCount) {
	// TM[1,7] successful completion of execution verification report
	assert(packetVersionNum <= 4 & packetType <= 1 & APID <= 1024 & seqFlag <= 2 &
	       packetSeqCount <= 8192);

	uint32_t value;

	value = (packetVersionNum << 29 | packetType << 28 | secondaryHeaderFlag << 27 | APID << 16 |
	         seqFlag << 14 | packetSeqCount);

	Message report = createTM(7);

	report.appendUint32(value);

	storeMessage(report);
}

void ReqVerifService::failExeVerif(uint8_t packetVersionNum, uint8_t packetType,
                                   bool secondaryHeaderFlag,
                                   uint16_t APID, uint8_t seqFlag, uint16_t packetSeqCount,
                                   uint16_t code) {
	// TM[1,8] failed completion of execution verification report
	assert(packetVersionNum <= 4 & packetType <= 1 & APID <= 1024 & seqFlag <= 2 &
	       packetSeqCount <= 8192);

	uint32_t value;

	value = (packetVersionNum << 29 | packetType << 28 | secondaryHeaderFlag << 27 | APID << 16 |
	         seqFlag << 14 | packetSeqCount);

	Message report = createTM(8);

	report.appendUint32(value);

	report.appendUint16(code);

	storeMessage(report);
}

void ReqVerifService::failRoutVerif(uint8_t packetVersionNum, uint8_t packetType,
                                    bool secondaryHeaderFlag,
                                    uint16_t APID, uint8_t seqFlag, uint16_t packetSeqCount,
                                    uint16_t code) {
	// TM[1,10] failed routing verification report
	assert(packetVersionNum <= 4 & packetType <= 1 & APID <= 1024 & seqFlag <= 2 &
	       packetSeqCount <= 8192);

	uint32_t value;

	value = (packetVersionNum << 29 | packetType << 28 | secondaryHeaderFlag << 27 | APID << 16 |
	         seqFlag << 14 | packetSeqCount);

	Message report = createTM(10);

	report.appendUint32(value);

	report.appendUint16(code);

	storeMessage(report);
}