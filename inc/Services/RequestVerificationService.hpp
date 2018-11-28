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
	void successAcceptanceVerification(Message &request);

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
	void failAcceptanceVerification(Message &request);


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
	void successExecutionVerification(Message &request);

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
	void failExecutionVerification(Message &request);

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
	void failRoutingVerification(Message &request);


};

#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
