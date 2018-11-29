#ifndef ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "Service.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * @todo All telemetry packets shall have a telemetry packet secondary header
 * @todo See if it would be more efficient to use Messages as arguments instead of individual
 * parameters
 */
class RequestVerificationService : public Service {
public:
	RequestVerificationService() {
		serviceType = 1;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 * Send report when the Cubesat accepts successfully commands
	 *
	 * Note:The parameters are the necessary information, defined from the standard, that the report
	 * should contain
	 *
	 * @param apid Application process ID
	 * @param seqFlag Sequence flags
	 * @param packetSeqCount Packet sequence count
	 */
	void successAcceptanceVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                   uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
	 * TM[1,2] failed acceptance verification report
	 *Send report when the Cubesat don't accept commands
	 *
	 * Note:The parameters are the necessary information, defined from the standard, that the report
	 * should contain
	 *
	 * @param apid Application process ID
	 * @param seqFlag Sequence flags
	 * @param packetSeqCount Packet sequence count
	 */
	void failAcceptanceVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                                uint16_t errorCode);


	/**
 	*  TM[1,7] successful completion of execution verification report
	 *  Send report when the Cubesat completes an execution
	 *
	 * Note:The parameters are the necessary information, defined from the standard, that the report
	 * should contain
	 *
	 * @param apid Application process ID
	 * @param seqFlag Sequence flags
	 * @param packetSeqCount Packet sequence count
 	*/
	void successExecutionVerification(Message::PacketType packetType, bool secondaryHeaderFlag,
	                                  uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount);

	/**
 	*  TM[1,8] failed completion of execution verification report
	 *  Send report when the Cubesat don't complete an execution
	 *
	 * Note:The parameters are the necessary information, defined from the standard, that the report
	 * should contain
	 *
	 * @param apid Application process ID
	 * @param seqFlag Sequence flags
	 * @param packetSeqCount Packet sequence count
 	*/
	void failExecutionVerification(Message::PacketType packetType,
	                               bool secondaryHeaderFlag,
	                               uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                               uint16_t errorCode);

	/**
 	*  TM[1,10] failed routing verification report
	 *  Send report when the routing of a request has failed
	 *
	 * Note:The parameters are the necessary information, defined from the standard, that the report
	 * should contain
	 *
	 * @param apid Application process ID
	 * @param seqFlag Sequence flags
	 * @param packetSeqCount Packet sequence count
 	*/
	void failRoutingVerification(Message::PacketType packetType,
	                             bool secondaryHeaderFlag,
	                             uint16_t apid, uint8_t seqFlag, uint16_t packetSeqCount,
	                             uint16_t errorCode);

	/**
	 * It is responsible to call the suitable function that execute the proper subservice. The
	 * way that the subservices are selected is for the time being based on the messageType(class
	 * member of class Message) of the param message
	 *
	 * Note:The functions of this service takes dummy values as arguments for the time being
	 *
	 * @todo Error handling for the switch() in the implementation of this execute function
	 */
	void execute(Message &message);

	/**
	 *  The purpose of this instance is to access the execute function of this service when a
	 *  MessageParser object is created
	 */
	static RequestVerificationService instance;
};


#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
