#ifndef ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Message.hpp"
#include "Service.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * Note:For the time being, the cause(routing, acceptance, execution), that functions of ST[01]
 * should be called, hasn't been implemented yet. In main.cpp there are some random calls with
 * dummy values.
 *
 * @todo See if the deduced data defined from the standard should still be ignored. This deduced
 * data exists only in reports that send failure signs(for example the TM[1,2])
 *
 * @ingroup Services
 */
class RequestVerificationService : public Service {
public:
	inline static const uint8_t ServiceType = 1;

	enum MessageType : uint8_t {
		SuccessfulAcceptanceReport = 1,
		FailedAcceptanceReport = 2,
		SuccessfulStartOfExecution = 3,
		FailedStartOfExecution = 4,
		SuccessfulProgressOfExecution = 5,
		FailedProgressOfExecution = 6,
		SuccessfulCompletionOfExecution = 7,
		FailedCompletionOfExecution = 8,
		FailedRoutingReport = 10,
	};

	RequestVerificationService() {
		serviceType = ServiceType;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that accepted successfully
	 */
	void successAcceptanceVerification(const Message& request);

	/**
	 * TM[1,2] failed acceptance verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic
	 * info of the telecommand packet that failed to be accepted
	 * @param errorCode The cause of creating this type of report
	 */
	void failAcceptanceVerification(const Message& request, ErrorHandler::AcceptanceErrorType errorCode);

	/**
	 * TM[1,3] successful start of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its start of execution is successful
	 */
	void successStartExecutionVerification(const Message& request);

	/**
	 * TM[1,4] failed start of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its start of execution has failed
	 * @param errorCode The cause of creating this type of report
	 */
	void failStartExecutionVerification(const Message& request, ErrorHandler::ExecutionStartErrorType errorCode);

	/**
	 * TM[1,5] successful progress of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its progress of execution is successful
	 * @param stepID If the execution of a request is a long process, then we can divide
	 * the process into steps. Each step goes with its own definition, the stepID.
	 * @todo Each value,that the stepID is assigned, should be documented.
	 * @todo error handling for undocumented assigned values to stepID
	 */
	void successProgressExecutionVerification(const Message& request, uint8_t stepID);

	/**
	 * TM[1,6] failed progress of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its progress of execution has failed
	 * @param errorCode The cause of creating this type of report
	 * @param stepID If the execution of a request is a long process, then we can divide
	 * the process into steps. Each step goes with its own definition, the stepID.
	 * @todo Each value,that the stepID is assigned, should be documented.
	 * @todo error handling for undocumented assigned values to stepID
	 */
	void failProgressExecutionVerification(const Message& request, ErrorHandler::ExecutionProgressErrorType errorCode,
	                                       uint8_t stepID);

	/**
	 * TM[1,7] successful completion of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually data members of Message that contain the basic info of the
	 * telecommand packet that executed completely and successfully
	 */
	void successCompletionExecutionVerification(const Message& request);

	/**
	 * TM[1,8] failed completion of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed to be executed completely
	 * @param errorCode The cause of creating this type of report
	 */
	void failCompletionExecutionVerification(const Message& request,
	                                         ErrorHandler::ExecutionCompletionErrorType errorCode);

	/**
	 * TM[1,10] failed routing verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed the routing
	 * @param errorCode The cause of creating this type of report
 	 */
	void failRoutingVerification(const Message& request, ErrorHandler::RoutingErrorType errorCode);
};

#endif // ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
