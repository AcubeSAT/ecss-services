#include "ECSS_Configuration.hpp"
#ifdef SERVICE_FUNCTION

#include "Services/FunctionManagementService.hpp"

void FunctionManagementService::call(Message& msg) {
	msg.resetRead();
	ErrorHandler::assertRequest(msg.packetType == Message::TC, msg,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(msg.messageType == FunctionManagementService::MessageType::PerformFunction, msg,
		ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(msg.serviceType == FunctionManagementService::ServiceType, msg,
		ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t funcName[ECSS_FUNCTION_NAME_LENGTH] = { 0 }; // the function's name
	uint8_t funcArgs[ECSS_FUNCTION_MAX_ARG_LENGTH] = { 0 }; // arguments for the function

	msg.readString(funcName, ECSS_FUNCTION_NAME_LENGTH);
	msg.readString(funcArgs, ECSS_FUNCTION_MAX_ARG_LENGTH);

	if (msg.dataSize > (ECSS_FUNCTION_NAME_LENGTH + ECSS_FUNCTION_MAX_ARG_LENGTH)) {
		ErrorHandler::reportError(msg,
		                          ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError); // report failed
		// start of execution as requested by the standard
		return;
	}

	// locate the appropriate function pointer
	String<ECSS_FUNCTION_NAME_LENGTH> name(funcName);
	FunctionMap::iterator iter = funcPtrIndex.find(name);
	void (*selected)(String<ECSS_FUNCTION_MAX_ARG_LENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	} else {
		ErrorHandler::reportError(msg, ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		return;
	}

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
}

void FunctionManagementService::include(String<ECSS_FUNCTION_NAME_LENGTH> funcName,
	void (* ptr)(String<ECSS_FUNCTION_MAX_ARG_LENGTH>)) {
	if (not funcPtrIndex.full()) { // CAUTION: etl::map won't check by itself if it's full
		// before attempting to insert a key-value pair, causing segmentation faults. Check first!
		funcName.append(ECSS_FUNCTION_NAME_LENGTH - funcName.length(), 0);
		funcPtrIndex.insert(std::make_pair(funcName, ptr));
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::MapFull);
	}
}

void FunctionManagementService::execute(Message& message) {
	switch (message.messageType) {
		case 1:
			call(message); // TC[8,1]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}

#endif
