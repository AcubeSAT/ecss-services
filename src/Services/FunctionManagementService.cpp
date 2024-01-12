#include "ECSS_Configuration.hpp"
#ifdef SERVICE_FUNCTION

#include "Services/FunctionManagementService.hpp"

void FunctionManagementService::call(Message& msg) {
	msg.resetRead();

	if (!msg.assertTC(ServiceType, MessageType::PerformFunction)) {
		return;
	}

	etl::array<uint8_t, ECSSFunctionNameLength> funcName = {0};
	etl::array<uint8_t, ECSSFunctionMaxArgLength> funcArgs = {0};

	msg.readString(funcName.data(), ECSSFunctionNameLength);
	msg.readString(funcArgs.data(), ECSSFunctionMaxArgLength);

	if (msg.dataSize > (ECSSFunctionNameLength + ECSSFunctionMaxArgLength)) {
		ErrorHandler::reportError(msg,
		                          ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		// start of execution as requested by the standard
		return;
	}

	// locate the appropriate function pointer
	String<ECSSFunctionNameLength> const name(funcName.data());
	FunctionMap::iterator const iter = funcPtrIndex.find(name); // NOLINT(cppcoreguidelines-init-variables)

	if (iter == funcPtrIndex.end()) {
		ErrorHandler::reportError(msg, ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		return;
	}

	auto selected = *iter->second;

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs.data());
}

void FunctionManagementService::include(String<ECSSFunctionNameLength> funcName,
                                        void (*ptr)(String<ECSSFunctionMaxArgLength>)) {
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
