#ifndef ECSS_SERVICES_REQEUSTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * @todo All telemetry packets shall have a telemetry packet secondary header
 */
class RequestVerificationService : public Service {
public:
	RequestVerificationService() {
		serviceType = 1;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 */
	void successAcceptanceVerification(uint8_t packetType, bool secondaryHeaderFlag,
	                                   uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
	 * TM[1,2] failed acceptance verification report
	 */
	void failAcceptanceVerification(uint8_t packetType, bool secondaryHeaderFlag,
	                                uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                                uint16_t errorCode);


	/**
 	*  TM[1,7] successful completion of execution verification report
 	*/
	void successExecutionVerification(uint8_t packetType, bool secondaryHeaderFlag,
	                                  uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
 	*  TM[1,8] failed completion of execution verification report
 	*/
	void failExecutionVerification(uint8_t packetType,
	                               bool secondaryHeaderFlag,
	                               uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                               uint16_t errorCode);

	/**
 	*  TM[1,10] failed routing verification report
 	*/
	void failRoutingVerification(uint8_t packetType,
	                             bool secondaryHeaderFlag,
	                             uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                             uint16_t errorCode);


};

#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
