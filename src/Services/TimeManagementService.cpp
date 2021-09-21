#include "ECSS_Configuration.hpp"
#include "Services/TimeManagementService.hpp"
#include <cmath>

void TimeManagementService :: decimal2binary(int n, int *arr, int numOctets) {

	int binaryNum[8 * numOctets];
	for (int i = 0; i < 8 * numOctets; i++) {
		binaryNum[i] = 0;
	}

	int i = 0;
	int k = 0;
	while (n > 0) {

		binaryNum[i] = n % 2;
		n = n / 2;
		i++;
	}

	for (int j = 8 * numOctets - 1; j >= 0; j--, k++) {
		arr[k] = binaryNum[j];
	}
}

template <typename T>
void TimeManagementService :: convert2cuc(T timestampMs, int numOfBasicOctets, int numOfFractionalOctets,
                                         Message& cucReport) {

	//TODO: take epoch into consideration.

	T timestampSec = timestampMs / 1000;
	auto fractionalTimeCountMax = static_cast <int64_t> (pow(2, 8 * numOfFractionalOctets));
	int64_t basicTimeCountMax = pow(2, 8 * numOfBasicOctets);

	int pField[8] = {0, 0, 0, 1, (numOfBasicOctets - 1) / 10, (numOfBasicOctets - 1) % 10, numOfFractionalOctets /
	        10, numOfFractionalOctets % 10};

	int tFieldSize = 8 * (numOfFractionalOctets + numOfBasicOctets);
	int tField[tFieldSize];
	for (int i = 0; i < tFieldSize; i++) {
		tField[i] = 0;
	}

	int fractionalTimeOffset = 8 * numOfBasicOctets;

	if (timestampSec < fractionalTimeCountMax) {

		decimal2binary(timestampSec, tField + fractionalTimeOffset, numOfFractionalOctets);
	}
	else {

		int basicTimeCount = timestampSec / fractionalTimeCountMax;
		int fractionalTimeCount = timestampSec % fractionalTimeCountMax;

		if (basicTimeCount > basicTimeCountMax) {
			//TODO: log an error.
		}

		decimal2binary(basicTimeCount, tField, numOfBasicOctets);
		decimal2binary(fractionalTimeCount, tField + fractionalTimeOffset, numOfFractionalOctets);
	}

	//Append pField and tField to the report, thus form the CUC format.
	for (int i : pField) {
		cucReport.appendUint64(static_cast <uint64_t> (i));
	}
	for(int i : tField) {
		cucReport.appendUint64(static_cast <uint64_t> (i));
	}
}

template <typename T>
void TimeManagementService :: convert2cds(T timestampMs, bool is16BitDaySegment, int subMsCodeBit1,
                                        int subMsCodeBit2, Message& cdsReport) {

	int64_t msOfDayCountMax = pow(2, 32);
	bool hasAbsentSubMsSegment = false;
	//bool isReserved = false;
	bool daySegmentCode = false;
	int subMsSegmentLength = 0;
	(is16BitDaySegment) ? daySegmentCode = false : daySegmentCode = true;

	int pField[8] = {0, 1, 0, 0, 0, static_cast <int> (daySegmentCode), subMsCodeBit1, subMsCodeBit2};

	if (subMsCodeBit1 == 0) {

		if (subMsCodeBit2 == 0) {
			hasAbsentSubMsSegment = true;
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
			//isReserved = true;
		}
	}

	int daySegmentOffset = 0;
	(daySegmentCode) ? daySegmentOffset = 24 : daySegmentOffset = 16;
	int tField[daySegmentOffset + subMsSegmentLength + 32];

	int64_t dayCountMax = pow(2, daySegmentOffset);

	if (timestampMs < msOfDayCountMax) {

		decimal2binary(timestampMs, tField + daySegmentOffset, 4);
	}
	else {

		int dayCount = timestampMs / msOfDayCountMax;
		int msOfDayCount = timestampMs % msOfDayCountMax;

		if (dayCount > dayCountMax) {
			//TODO: log an error.
		}

		decimal2binary(dayCount, tField, daySegmentOffset / 8);
		decimal2binary(msOfDayCount, tField + daySegmentOffset, 4);
	}

	for (int i : pField) {
		cdsReport.appendUint8(i);
	}
	for (int i : tField) {
		cdsReport.appendUint8(i);
	}

	if (!hasAbsentSubMsSegment) {

		//TODO: Add sub-milliseconds at the end of the report.
	}
}

void TimeManagementService :: setTimerReportGenerationRate(Message& rateExponentialValue) {

	ErrorHandler::assertRequest(rateExponentialValue.packetType == Message::TC, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.messageType == TimeManagementService::MessageType::SetTimeReportGenerationRate,
	                            rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.serviceType == TimeManagementService::ServiceType, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	int newRate = rateExponentialValue.readUint16();

	// Check overflow as described in ECSS Datasheet
	ErrorHandler::assertRequest(newRate <= 8, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	newRate = static_cast <int> (pow(2, newRate));

	TimeManagementService :: timeReportGenerationRate = newRate;
}

void TimeManagementService :: cucTimeReport(Message& timestampMs) {
// This function has to be called with the specified rate.

	int timeReferenceStatus = 0;  //What is this??

	Message cucReport(TimeManagementService::ServiceType, TimeManagementService::MessageType::CucTimeReport,
	                  Message::TM, 1);

	ErrorHandler::assertRequest(timestampMs.packetType == Message::TC, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.messageType == TimeManagementService::MessageType::CucTimeReport,
	                            timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.serviceType == TimeManagementService::ServiceType, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	cucReport.appendUint16(static_cast <int> (log2(TimeManagementService::timeReportGenerationRate)));

	// Do the conversion and store it into the report message.
	int timestampToConvert = timestampMs.readUint16();
	int numOfBasicOctets = 4;
	int numOfFractionalOctets = 1;
	convert2cuc(timestampToConvert, numOfBasicOctets, numOfFractionalOctets, cucReport);
	cucReport.appendUint16(timeReferenceStatus);

	storeMessage(cucReport);

}

void TimeManagementService :: cdsTimeReport(Message& timestampMs) {
//This function has to be called with the specified rate.

	int timeReferenceStatus = 0;   //???

	Message cdsReport(TimeManagementService::ServiceType, TimeManagementService::MessageType::CdsTimeReport,
	                  Message::TM, 1);

	ErrorHandler::assertRequest(timestampMs.packetType == Message::TC, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.messageType == TimeManagementService::MessageType::CdsTimeReport,
	                            timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timestampMs.serviceType == TimeManagementService::ServiceType, timestampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	cdsReport.appendUint16(static_cast <int> (log2(TimeManagementService::timeReportGenerationRate)));

	int timestampToConvert = timestampMs.readUint16();
	bool is16BitDaySegment = true;
	int subMsCodeBit1 = 0;
	int subMsCodeBit2 = 0;
	convert2cds(timestampToConvert, is16BitDaySegment, subMsCodeBit1, subMsCodeBit2, cdsReport);
	cdsReport.appendUint16(timeReferenceStatus);

	storeMessage(cdsReport);
}

