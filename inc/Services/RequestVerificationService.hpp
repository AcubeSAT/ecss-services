#ifndef ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
#define ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP

#include "Service.hpp"
#include "Message.hpp"
#include "ErrorHandler.hpp"
#include "ECSS_Definitions.hpp"

/**
 * Implementation of the ST[01] request verification service
 *
 * Note:For the time being, the cause(routing, acceptance, execution), that functions of ST[01]
 * should be called, hasn't been implemented yet. In main.cpp there are some random calls with
 * dummy values.
 *
 * @todo See if the deduced data defined from the standard should still be ignored. This deduced
 * data exists only in reports that send failure signs(for example the TM[1,2])
 */
class RequestVerificationService : public Service {
public:
	RequestVerificationService() {
		serviceType = 1;
	}

	/**
	 * TM[1,1] successful acceptance verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that accepted successfully
	 */
	void successAcceptanceVerification(const Message &request);

	/**
	 * TM[1,2] failed acceptance verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic
	 * info of the telecommand packet that failed to be accepted
	 * @param errorCode The cause of creating this type of report
	 */
	void failAcceptanceVerification(const Message &request, ErrorHandler::AcceptanceErrorType
	errorCode);

	/**
	 * TM[1,3] successful start of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its start of execution is successful
	 */
	void successStartExecutionVerification(const Message &request);

	/**
	 * TM[1,4] failed start of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its start of execution has failed
	 * @param errorCode The cause of creating this type of report
	 */
	void failStartExecutionVerification(const Message &request,
		ErrorHandler::ExecutionStartErrorType
	errorCode);

	/**
	 * TM[1,5] successful progress of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its progress of execution is successful
	 * @param step Step identifier
	 */
	void successProgressExecutionVerification(const Message &request, ErrorHandler::stepID step);

	/**
	 * TM[1,6] failed progress of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info
	 * of the telecommand packet that its progress of execution has failed
	 * @param errorCode The cause of creating this type of report
	 * @param step Step identifier
	 */
	void failProgressExecutionVerification(const Message &request,
		ErrorHandler::ExecutionProgressErrorType errorCode, ErrorHandler::stepID step);

	/**
 	 * TM[1,7] successful completion of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually data members of Message that contain the basic info of the
	 * telecommand packet that executed completely and successfully
 	 */
	void successCompletionExecutionVerification(const Message &request);

	/**
	 * TM[1,8] failed completion of execution verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed to be executed completely
	 * @param errorCode The cause of creating this type of report
	 */
	void failCompletionExecutionVerification(const Message &request,
		ErrorHandler::ExecutionCompletionErrorType errorCode);

	/**
	 * TM[1,10] failed routing verification report
	 *
	 * @param request Contains the necessary data to send the report.
	 * The data is actually some data members of Message that contain the basic info of the
	 * telecommand packet that failed the routing
	 * @param errorCode The cause of creating this type of report
 	 */
	void failRoutingVerification(const Message &request, ErrorHandler::RoutingErrorType errorCode);

	/**
	 * It is responsible to call the suitable function that execute the proper subservice. The
	 * way that the subservices are selected is for the time being based on the messageType(class
	 * member of class Message) of the param message
	 *
	 * Note:The functions of this service takes dummy values as arguments for the time being
	 *
	 * @todo execute() needs to be redefined. The /p message isn't enough to call some
	 * subservices. More arguments are needed.
	 */
	void execute(const Message &message);

	/**
	 * The purpose of this instance is to access the execute function of this service when a
	 * MessageParser object is created
	 */
	static RequestVerificationService instance;
};


#endif //ECSS_SERVICES_REQUESTVERIFICATIONSERVICE_HPP
