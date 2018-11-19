#ifndef ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * @todo All telemetry packets shall have a telemetry packet secondary header
 * @todo See if it would be more efficient to use Messages as arguments instead of individual parameters
 */
class RequestVerificationService : public Service {
public:
	RequestVerificationService() {
		serviceType = 1;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 */
	void successAcceptanceVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                   uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
	 * TM[1,2] failed acceptance verification report
	 */
	void failAcceptanceVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                                uint16_t errorCode);


	/**
 	*  TM[1,7] successful completion of execution verification report
 	*/
	void successExecutionVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                  uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
 	*  TM[1,8] failed completion of execution verification report
 	*/
	void failExecutionVerification(Message::PacketType packetType,
	                               bool secondaryHeaderFlag,
	                               uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                               uint16_t errorCode);

	/**
 	*  TM[1,10] failed routing verification report
 	*/
	void failRoutingVerification(Message::PacketType packetType,
	                             bool secondaryHeaderFlag,
	                             uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                             uint16_t errorCode);


};

#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
