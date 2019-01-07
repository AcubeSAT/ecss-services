#include "Services/FunctionManagementService.hpp"

#ifdef TESTMODE
//void foo(String<MAXARGLENGTH> b) {
//	std::cout << "SPAAAACE!" << std::endl;
//}
//
//void bar(String<MAXARGLENGTH> b) {
//	std::cout << "I HAZ A CUBESAT THAT SNAPS PIX!" << std::endl;
//}
//
//void baz(String<MAXARGLENGTH> b) {
//	std::cout << "QWERTYUIOP" << std::endl;
//}
//
//void dummy1(const String<MAXARGLENGTH> a) {
//	std::cout << a.c_str() << std::endl;
//}
//
//FunctionManagementService::FunctionManagementService() {
//	// Sample inclusion of functions in the pointer map.
//	include(String<FUNCNAMELENGTH>("dummy1"), &dummy1);
//	include(String<FUNCNAMELENGTH>("foo"), &foo);
//	include(String<FUNCNAMELENGTH>("bar"), &bar);
//	include(String<FUNCNAMELENGTH>("baz"), &baz);
//	// All the functions that should be included in the pointer map at initialization shall be here.
//}
#else
FunctionManagementService::FunctionManagementService() {
	// INSERT YOUR OWN FUNCTIONS HERE AS ABOVE!
}
#endif

int FunctionManagementService::call(Message msg){
	assert(msg.messageType == 1);
	assert(msg.serviceType == 8);

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
	void(*selected)(String<MAXARGLENGTH>);

	if (iter != funcPtrIndex.end()) {
		selected = *iter->second;
	}
	else {
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
