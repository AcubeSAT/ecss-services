#include "ECSS_Configuration.hpp"
#include "Services/TimeManagementService.hpp"
#include <cmath>

void TimeManagementService :: decimal2binary(uint16_t n, uint8_t *arr, int numOctets) {

	uint8_t binaryNum[8 * numOctets];
	for (int i = 0; i < 8 * numOctets; i++) binaryNum[i] = 0;

	uint16_t i = 0;
	uint16_t k = 0;
	while (n > 0) {

		binaryNum[i] = n % 2;
		n = n / 2;
		i++;
	}

	for (int j = 8 * numOctets - 1; j >= 0; j--, k++) {
		arr[k] = binaryNum[j];
	}
}

void TimeManagementService :: convert2cuc(uint16_t timestampMs, int numOfBasicOctets, int numOfFractionalOctets,
                                         Message& cucReport) {

	//TODO: take epoch into consideration.

	uint16_t timestampSec = timestampMs / 1000;
	int fractionalTimeCountMax = (int) pow(2, 8 * numOfFractionalOctets);
	long basicTimeCountMax = pow(2, 8 * numOfBasicOctets);

	int pField[8] = {0, 0, 0, 1, (numOfBasicOctets - 1) / 10, (numOfBasicOctets - 1) % 10, numOfFractionalOctets /
	        10, numOfFractionalOctets % 10};

	uint16_t tFieldSize = 8 * (numOfFractionalOctets + numOfBasicOctets);
	uint8_t tField[tFieldSize];
	for (int i = 0; i < tFieldSize; i++) {
		tField[i] = 0;
	}

	uint16_t fractionalTimeOffset = 8 * numOfBasicOctets;

	if (timestampSec < fractionalTimeCountMax) {

		decimal2binary(timestampSec, tField + fractionalTimeOffset, numOfFractionalOctets);
	}
	else {

		uint16_t basicTimeCount = timestampSec / fractionalTimeCountMax;
		uint16_t fractionalTimeCount = timestampSec % fractionalTimeCountMax;

		//TODO: check if basicTimeCount exceeds basicTimeCountMax and log the error.

		decimal2binary(basicTimeCount, tField, numOfBasicOctets);
		decimal2binary(fractionalTimeCount, tField + fractionalTimeOffset, numOfFractionalOctets);
	}

	//Append pField and tField to the report, thus form the CUC format.
	for (int i : pField) {
		cucReport.appendUint8((uint8_t) i);
	}
	for(int i : tField) {
		cucReport.appendUint8((uint8_t) i);
	}
}

void TimeManagementService :: convert2cds(uint16_t timestampMs, bool is16BitDaySegment, uint8_t subMsCodeBit1,
                                        uint8_t subMsCodeBit2, Message& cdsReport) {

	long msOfDayCountMax = pow(2, 32);
	bool isAbsent = false;
	bool isReserved = false;
	bool daySegmentCode = 0;
	uint8_t subMsSegmentLength = 0;
	daySegmentCode = (is16BitDaySegment == true) ? 0 : 1;

	uint8_t pField[8] = {0, 1, 0, 0, 0, daySegmentCode, subMsCodeBit1, subMsCodeBit2};

	if (subMsCodeBit1 == 0) {

		if (subMsCodeBit2 == 0) {
			isAbsent = true;
		}
		else {
			subMsSegmentLength = 16;
		}
	}
	else if (subMsCodeBit1 == 1) {

		if (subMsCodeBit2 == 0) {
			subMsSegmentLength = 32;
		}
		else {
			isReserved = true;
		}
	}

	uint8_t daySegmentOffset = (daySegmentCode == true) ? 24 : 16;
	uint8_t tField[daySegmentOffset + subMsSegmentLength + 32];

	int dayCountMax = pow(2, daySegmentOffset);

	if (timestampMs < msOfDayCountMax) {

		decimal2binary(timestampMs, tField + daySegmentOffset, 4);
	}
	else {

		uint8_t dayCount = timestampMs / msOfDayCountMax;
		uint8_t msOfDayCount = timestampMs % msOfDayCountMax;

		//TODO: check if dayCount exceeds dayCountMax and log the error.

		decimal2binary(dayCount, tField, daySegmentOffset / 8);
		decimal2binary(msOfDayCount, tField + daySegmentOffset, 4);
	}

	for (int i : pField) {
		cdsReport.appendUint8(i);
	}
	for (int i : tField) {
		cdsReport.appendUint8(i);
	}

	//TODO: if subMs segment is not absent, add sub-milliseconds at the end of the report.

}

void TimeManagementService :: setTimerReportGenerationRate(Message& rateExponentialValue) {

	ErrorHandler::assertRequest(rateExponentialValue.packetType == Message::TC, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.messageType == TimeManagementService::MessageType::SetTimeReportGenerationRate,
	                            rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.serviceType == TimeManagementService::ServiceType, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t newRate = rateExponentialValue.readUint16();

	// Check overflow as described in ECSS Datasheet
	ErrorHandler::assertRequest(newRate <= 8, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	newRate = (uint16_t) pow(2, newRate);

	TimeManagementService :: timeReportGenerationRate = newRate;
}

void TimeManagementService :: cucTimeReport(Message& timestampMs) {
// This function has to be called with the specified rate.

	uint16_t timeReferenceStatus = 0;  //What is this??

	Message cucReport(TimeManagementService::ServiceType, TimeManagementService::MessageType::CucTimeReport,
	                  Message::TM, 1);

	ErrorHandler::assertRequest(timestampMs.packetType == Message::TC, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.messageType == TimeManagementService::MessageType::CucTimeReport,
	                            timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.serviceType == TimeManagementService::ServiceType, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	cucReport.appendUint16((uint16_t) log2(TimeManagementService::timeReportGenerationRate));

	// Do the conversion and store it into the report message.
	uint16_t timestampToConvert = timestampMs.readUint16();
	int numOfBasicOctets = 4;
	int numOfFractionalOctets = 1;
	convert2cuc(timestampToConvert, numOfBasicOctets, numOfFractionalOctets, cucReport);
	cucReport.appendUint16(timeReferenceStatus);

	storeMessage(cucReport);

}

void TimeManagementService :: cdsTimeReport(Message& timestampMs) {
//This function has to be called with the specified rate.

	uint16_t timeReferenceStatus = 0;   //???

	Message cdsReport(TimeManagementService::ServiceType, TimeManagementService::MessageType::CdsTimeReport,
	                  Message::TM, 1);

	ErrorHandler::assertRequest(timestampMs.packetType == Message::TC, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.messageType == TimeManagementService::MessageType::CdsTimeReport,
	                            timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.serviceType == TimeManagementService::ServiceType, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	cdsReport.appendUint16((uint16_t) log2(TimeManagementService::timeReportGenerationRate));

	uint16_t timestampToConvert = timestampMs.readUint16();
	bool is16BitDaySegment = true;
	uint8_t subMsCodeBit1 = 0;
	uint8_t subMsCodeBit2 = 0;
	convert2cds(timestampToConvert, is16BitDaySegment, subMsCodeBit1, subMsCodeBit2, cdsReport);
	cdsReport.appendUint16(timeReferenceStatus);

	storeMessage(cdsReport);
}

