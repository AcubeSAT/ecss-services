#ifndef ECSS_SERVICES_TIMEMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_TIMEMANAGEMENTSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"


class TimeManagementService : public Service {
public:

	inline static const uint8_t ServiceType = 9;

	TimeManagementService() = default;

	enum MessageType : uint8_t {
		SetTimeReportGenerationRate = 1,
		CucTimeReport = 2,
		CdsTimeReport = 3,
	};


	// Should be 2,4,8,16,32,64,128 or 256
	uint16_t timeReportGenerationRate = 8;
	bool containsPfield = true;

	/**
	 * This function converts a number from decimal to binary
	 * and stores it into array
	 * @param n: decimal number to be converted.
	 * @param arr: array to store the binary result.
	 * @param numOctets: number of octets. Needed for array size.
	 */
	void decimal2binary(uint16_t n, uint8_t *arr, int numOctets);

	/**
	 * This function takes a timestamp (ms) as argument
	 * and converts it to cuc format.
	 *
	 * @param timestampMs: the timestamp in ms.
	 * @param numOfBasicOctets: the number of basic time octets.
	 * @param numOfFractionalOctets: the number of fractional time octets
	 * @param cucReport: the report of type message that contains the CUC format.
	 */
	void convert2cuc(uint16_t timestampMs, int numOfBasicOctets, int numOfFractionalOctets, Message& cucReport);

	/**
	 * This function takes a timestamp (ms) as argument
	 * and converts it to cds format.
	 *
	 * @param timestampMs: the timestamp in ms.
	 * @param is16BitDaySegment:
	 * @param subMsCodeBit1:
	 * @param subMsCodebit2:
	 * @param cdsReport: the report of type message that contains the CDS format.
	 */
	void convert2cds(uint16_t timestampMs, bool is16BitDaySegment, uint8_t subMsCodeBit1, uint8_t subMsCodeBit2,
	                 Message& cdsReport);

	/**
	 * This function receives a TC[9,1] message containing a requested
	 * rate value, and if it's valid, it sets the time report generation
	 * rate to this value.
	 *
	 * @param rateExponentialValue: number 2 raised in this value gives
	 * 		   						the new rate to be set.
	 */
	void setTimerReportGenerationRate(Message& rateExponentialValue);

	/**
	 * This function converts a timestamp (ms) to CUC format and
	 * reports it.
	 *
	 * @param timestampMs: timestamp (ms) to be converted to CUC.
	 */
	void cucTimeReport(Message& timestampMs);

	/**
	 * This function converts a timestamp (ms) to CDS format and
	 * reports it.
	 *
	 * @param timestampMs: timestamp (ms) to be converted to CDS.
	 */
	void cdsTimeReport(Message& timestampMs);

};

#endif