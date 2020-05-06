#include "ECSS_Configuration.hpp"
#ifdef SERVICE_LARGEPACKET

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

void LargePacketTransferService::split(Message& message, uint16_t largeMessageTransactionIdentifier) {
	//TODO: Should this be uint32?
	uint16_t size = message.dataSize;
	uint16_t positionCounter = 0;
	uint16_t parts = (size/ECSS_MAX_FIXED_OCTET_STRING_SIZE) + 1;
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> stringPart("");
	uint8_t dataPart[ECSS_MAX_FIXED_OCTET_STRING_SIZE];

	for (uint16_t i = 0; i < ECSS_MAX_FIXED_OCTET_STRING_SIZE; i++){
		dataPart[i] = message.data[positionCounter];
		positionCounter++;
	}
	stringPart = dataPart;
	firstDownlinkPartReport(largeMessageTransactionIdentifier, 0, stringPart);

	for (uint16_t part = 1; part < (parts - 1U); part++){
		for (uint16_t i = 0; i < ECSS_MAX_FIXED_OCTET_STRING_SIZE; i++){
			dataPart[i] = message.data[positionCounter];
			positionCounter++;
		}
		stringPart = dataPart;
		intermediateDownlinkPartReport(largeMessageTransactionIdentifier, part, stringPart);
	}

	for (uint16_t i = 0; i < ECSS_MAX_FIXED_OCTET_STRING_SIZE; i++){
		if (message.dataSize == positionCounter){
			dataPart[i] = 0; // To prevent from filling the rest of the String with garbage info
		}
		dataPart[i] = message.data[positionCounter];
		positionCounter++;
	}
	stringPart = dataPart;
	lastDownlinkPartReport(largeMessageTransactionIdentifier, (parts - 1U), stringPart);
}

#endif
