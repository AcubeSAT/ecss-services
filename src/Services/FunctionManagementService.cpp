#include "Services/FunctionManagementService.hpp"

int FunctionManagementService::call(Message& msg) {
	/**
	 * @todo: Add test for message and service type using the ErrorHandler
	 * @todo: Convert all functions to void (use error reports for tests instead of return numbers)
	 */
	msg.resetRead();
	ErrorHandler::assertInternal(msg.messageType == 1 && msg.serviceType == 8,
	                             ErrorHandler::InternalErrorType::UnacceptablePacket);

	uint8_t funcName[FUNC_NAME_LENGTH];  // the function's name
	uint8_t funcArgs[MAX_ARG_LENGTH];    // arguments for the function

	msg.readString(funcName, FUNC_NAME_LENGTH);
	msg.readString(funcArgs, MAX_ARG_LENGTH);

	if (msg.dataSize > FUNC_NAME_LENGTH + MAX_ARG_LENGTH) {
		/**
		 * @todo Send failed start of execution (too long message)
		 */
		return 4;  // arbitrary
	}

	// locate the appropriate function pointer
	String<FUNC_NAME_LENGTH> name(funcName);
	FunctionMap::iterator iter = funcPtrIndex.find(name);
	void (*selected)(String<MAX_ARG_LENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	} else {
		/**
		 * @todo Send failed start of execution (function not found)
		 */
		return 1;  // arbitrary
	}

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
	return 0;
}

int FunctionManagementService::include(String<FUNC_NAME_LENGTH> funcName, void(*ptr)
	(String<MAX_ARG_LENGTH>)) {

	if (funcPtrIndex.full()) {
		/**
		 * @todo Generate suitable notification (index is full)
		 */
		return 2;  // arbitrary, for testing purposes
	}

	funcName.append(FUNC_NAME_LENGTH - funcName.length(), '\0');
	funcPtrIndex.insert(std::make_pair(funcName, ptr));

	return 0;
}
