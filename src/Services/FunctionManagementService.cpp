#include "Services/FunctionManagementService.hpp"

//void dummy1(const String<MAXARGLENGTH> a) {
//	std::cout << a.c_str() << std::endl;
//}

/*
FunctionManagementService::FunctionManagementService() {
	// Sample inclusion of a function in the pointer map.
	// include(String<MAXFUNCNAMELENGTH>("dummy1"), &dummy1);

	// All the functions that should be included in the pointer map at initialization shall be here.
}
*/

#ifdef TESTMODE
int FunctionManagementService::call(Message msg){
	assert(msg.messageType == 1);
	assert(msg.serviceType == 8);

	uint8_t funcName[MAXFUNCNAMELENGTH];  // the function's name
	uint8_t funcArgs[MAXARGLENGTH];    // arguments for the function

	// initialize the function name and the argument arrays
	for (int i = 0; i < MAXFUNCNAMELENGTH; i++) {
		funcName[i] = '\0';
		funcArgs[i] = '\0';
	}

	// isolate the function's name from the incoming message
	for (int i = 0; i < MAXFUNCNAMELENGTH; i++) {
		uint8_t currByte = msg.readByte();
		if (currByte == 0x20) {
			continue;
		}
		funcName[i] = currByte;
	}

	// isolate the string containing the args (if string length exceeds max, the remaining bytes
	// are silently ignored)
	for (int i = 0; i < MAXARGLENGTH; i++) {
		uint8_t currByte = msg.readByte();
		if (currByte == 0x20) {
			continue;
		}
		funcArgs[i] = currByte;
	}

	// locate the appropriate function pointer
	String<MAXFUNCNAMELENGTH> name(funcName);
	PointerMap::iterator iter = funcPtrIndex.find(name);
	void(*selected)(String<MAXARGLENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	}
	else {
		/**
		 * @todo Send failed start of execution
		 */
		return 1;
	}

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
	return 0;
}
#else
void FunctionManagementService::call(Message msg){
	assert(msg.messageType == 1);
	assert(msg.serviceType == 8);

	uint8_t funcName[MAXFUNCNAMELENGTH];  // the function's name
	uint8_t funcArgs[MAXARGLENGTH];    // arguments for the function

	// initialize the function name and the argument arrays
	for (int i = 0; i < MAXFUNCNAMELENGTH; i++) {
		funcName[i] = '\0';
		funcArgs[i] = '\0';
	}

	// isolate the function's name from the incoming message
	for (int i = 0; i < MAXFUNCNAMELENGTH; i++) {
		uint8_t currByte = msg.readByte();
		if (currByte == 0x20) {
			continue;
		}
		funcName[i] = currByte;
	}

	// isolate the string containing the args (if string length exceeds max, the remaining bytes
	// are silently ignored)
	for (int i = 0; i < MAXARGLENGTH; i++) {
		uint8_t currByte = msg.readByte();
		if (currByte == 0x20) {
			continue;
		}
		funcArgs[i] = currByte;
	}

	// locate the appropriate function pointer
	String<MAXFUNCNAMELENGTH> name(funcName);
	PointerMap::iterator iter = funcPtrIndex.find(name);
	void(*selected)(String<MAXARGLENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	}
	else {
		/**
		 * @todo Send failed start of execution
		 */
		return;
	}

	// execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
}

void FunctionManagementService::include(String<MAXFUNCNAMELENGTH> funcName,
	void (*ptr)(String<MAXARGLENGTH>)) {

	if (funcName.length() <= MAXFUNCNAMELENGTH) {
		funcName.append(MAXFUNCNAMELENGTH - funcName.length(), '\0');
	}
	else {
		return;
	}

	funcPtrIndex.insert(std::make_pair(funcName, ptr));
}
#endif

void FunctionManagementService::include(String<MAXFUNCNAMELENGTH> funcName, void(*ptr)
(String<MAXARGLENGTH>)) {

	if (funcName.length() <= MAXFUNCNAMELENGTH) {
		funcName.append(MAXFUNCNAMELENGTH - funcName.length(), '\0');
	}

	funcPtrIndex.insert(std::make_pair(funcName, ptr));
}
