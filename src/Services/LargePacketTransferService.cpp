#include <Services/LargePacketTransferService.hpp>
#include "Message.hpp"
#include <etl/String.hpp>

void LargePacketTransferService::firstDownlinkPartReport(uint16_t largeMessageTransactionIdentifier,
                                                         uint16_t partSequenceNumber,
                                                         const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TM[13,1]

	Message report = createTM(1);
	report.appendUint16(largeMessageTransactionIdentifier); // large message transaction identifier
	report.appendUint16(partSequenceNumber); // part sequence number
	report.appendOctetString(string); // fixed octet-string
	storeMessage(report);
}

void LargePacketTransferService::intermediateDownlinkPartReport(
    uint16_t largeMessageTransactionIdentifier, uint16_t partSequenceNumber,
    const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TM[13,2]
	Message report = createTM(2);
	report.appendUint16(largeMessageTransactionIdentifier); // large message transaction identifier
	report.appendUint16(partSequenceNumber); // part sequence number
	report.appendOctetString(string); // fixed octet-string
	storeMessage(report);
}

void LargePacketTransferService::lastDownlinkPartReport(uint16_t largeMessageTransactionIdentifier,
                                                        uint16_t partSequenceNumber,
                                                        const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TM[13,3]
	Message report = createTM(3);
	report.appendUint16(largeMessageTransactionIdentifier); // large message transaction identifier
	report.appendUint16(partSequenceNumber); // part sequence number
	report.appendOctetString(string); // fixed octet-string
	storeMessage(report);
}

String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>
LargePacketTransferService::firstUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TC[13,9]
	return string;
}

String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>
LargePacketTransferService::intermediateUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TC[13,10]
	return string;
}

String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>
LargePacketTransferService::lastUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string) {
	// TC[13, 11]
	return string;
}
