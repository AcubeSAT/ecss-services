#include "Services/FunctionManagementService.hpp"

// Dummy functions which will populate the map.
// This one prints whatever bytes are contained in the argument.
void dummy1(const String<MAXARGLENGTH> a) {
	std::cout << a.c_str() << std::endl;
}


FunctionManagementService::FunctionManagementService() {
	String<MAXFUNCNAMELENGTH> str("");
	str.append("dummy1");
	str.append(MAXFUNCNAMELENGTH - 6, '\0');
	void(*dummyPtr)(String<MAXARGLENGTH>) = &dummy1;
	funcPtrIndex.insert(std::make_pair(str, dummyPtr));
}

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
	void(*selected)(String<MAXARGLENGTH>) = nullptr;

	if (iter == funcPtrIndex.end()) {
		std::cout << "ERROR: Malformed query." << std::endl;
	}
	else {
		selected = *iter->second;
	}

	// send proper exec failure notification and terminate if name is incorrect
	if (selected == nullptr) {
		/**
		 * @todo Send failed start of execution
		 */
		return;
	}

	//  execute the function if there are no obvious flaws (defined in the standard, pg.158)
	selected(funcArgs);
}
