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

	uint8_t funcName[ECSSFunctionNameLength] = { 0 }; // the function's name
	uint8_t funcArgs[ECSSFunctionMaxArgLength] = { 0 }; // arguments for the function

	msg.readString(funcName, ECSSFunctionNameLength);
	msg.readString(funcArgs, ECSSFunctionMaxArgLength);

	if (msg.dataSize > (ECSSFunctionNameLength + ECSSFunctionMaxArgLength)) {
		ErrorHandler::reportError(msg,
		                          ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError); // report failed
		// start of execution as requested by the standard
		return;
	}

	// locate the appropriate function pointer
	String<ECSSFunctionNameLength> name(funcName);
	FunctionMap::iterator iter = funcPtrIndex.find(name);

	if (iter == funcPtrIndex.end()) {
		ErrorHandler::reportError(msg, ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		return;
	}

	auto selected = *iter->second;

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
}

void FunctionManagementService::include(String<ECSSFunctionNameLength> funcName,
	void (* ptr)(String<ECSSFunctionMaxArgLength>)) {
	if (not funcPtrIndex.full()) { // CAUTION: etl::map won't check by itself if it's full
		// before attempting to insert a key-value pair, causing segmentation faults. Check first!
		funcName.append(ECSSFunctionNameLength - funcName.length(), 0);
		funcPtrIndex.insert(std::make_pair(funcName, ptr));
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::MapFull);
	}
}

void FunctionManagementService::execute(Message& message) {
	switch (message.messageType) {
		case PerformFunction:
			call(message); // TC[8,1]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}

#endif
