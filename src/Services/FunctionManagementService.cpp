#include "Services/FunctionManagementService.hpp"

/**
 * Usage of the include() function:
 *
 * void foo(String<MAXARGLENGTH> b) {
 * 		std::cout << "SPAAAACE!" << std::endl;
 * 	}
 *
 * void bar(String<MAXARGLENGTH> b) {
 * 		std::cout << "I HAZ A CUBESAT THAT SNAPS PIX!" << std::endl;
 * 	}
 *
 * void baz(String<MAXARGLENGTH> b) {
 * 		std::cout << "QWERTYUIOP" << std::endl;
 * 	}
 *
 * 	FunctionManagementService::FunctionManagementService() {
 * 		include(String<FUNCNAMELENGTH>("foo"), &foo);
 * 		include(String<FUNCNAMELENGTH>("bar"), &bar);
 * 		include(String<FUNCNAMELENGTH>("baz"), &baz);
 * 	}
 */

/*
 * FunctionManagementService::FunctionManagementService() {
 * All the functions that should be included in the pointer map at initialization shall be here.
 * and included as in the examples above.
 *
 * }
 */

int FunctionManagementService::call(Message msg) {
	ErrorHandler::assertInternal(msg.messageType == 1 && msg.serviceType == 8,
	                             ErrorHandler::InternalErrorType::UnacceptablePacket);

	uint8_t funcName[FUNCNAMELENGTH];  // the function's name
	uint8_t funcArgs[MAXARGLENGTH];    // arguments for the function

	msg.readString(funcName, FUNCNAMELENGTH);
	msg.readString(funcArgs, MAXARGLENGTH);

	if (msg.dataSize > FUNCNAMELENGTH + MAXARGLENGTH) {
		/**
		 * @todo Send failed start of execution (too long message)
		 */
		return 4;  // arbitrary
	}

	// locate the appropriate function pointer
	String<FUNCNAMELENGTH> name(funcName);
	FunctionMap::iterator iter = funcPtrIndex.find(name);
	void (*selected)(String<MAXARGLENGTH>);

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

int FunctionManagementService::include(String<FUNCNAMELENGTH> funcName, void(*ptr)
	(String<MAXARGLENGTH>)) {

	if (funcPtrIndex.full()) {
		/**
		 * @todo Generate suitable notification (index is full)
		 */
		return 2;  // arbitrary, for testing purposes
	}

	funcName.append(FUNCNAMELENGTH - funcName.length(), '\0');
	funcPtrIndex.insert(std::make_pair(funcName, ptr));

	return 0;
}
