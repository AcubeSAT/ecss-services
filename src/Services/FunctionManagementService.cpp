#include "Services/FunctionManagementService.hpp"

void FunctionManagementService::call(Message& msg) {
	msg.resetRead();
	ErrorHandler::assertRequest(msg.packetType == Message::TC, msg,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(msg.messageType == 1, msg, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(msg.serviceType == 8, msg, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t funcName[FUNC_NAME_LENGTH]; // the function's name
	uint8_t funcArgs[MAX_ARG_LENGTH]; // arguments for the function

	msg.readString(funcName, FUNC_NAME_LENGTH);
	msg.readString(funcArgs, MAX_ARG_LENGTH);

	if (msg.dataSize > (FUNC_NAME_LENGTH + MAX_ARG_LENGTH)) {
		ErrorHandler::reportError(msg,
		                          ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError); // report failed
		// start of execution as requested by the standard
		return;
	}

	// locate the appropriate function pointer
	String<FUNC_NAME_LENGTH> name(funcName);
	FunctionMap::iterator iter = funcPtrIndex.find(name);
	void (*selected)(String<MAX_ARG_LENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	} else {
		ErrorHandler::reportError(msg, ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		return;
	}

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
}

void FunctionManagementService::include(String<FUNC_NAME_LENGTH> funcName, void (*ptr)(String<MAX_ARG_LENGTH>)) {
	if (not funcPtrIndex.full()) { // CAUTION: etl::map won't check by itself if it's full
		// before attempting to insert a key-value pair, causing segmentation faults. Check first!
		funcName.append(FUNC_NAME_LENGTH - funcName.length(), '\0');
		funcPtrIndex.insert(std::make_pair(funcName, ptr));
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::FunctionMapFull);
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
