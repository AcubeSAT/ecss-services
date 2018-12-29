#include "Services/FunctionManagementService.hpp"

// Dummy functions which will populate the map
float dummy1(float a, int b) {
	return a * b;
}

void dummy2(char c) {
	std::cout << c << std::endl;
}

//FunctionManagementService::FunctionManagementService() {
//	funcPtrIndex.insert(std::make_pair(String<MAXFUNCNAMELEN>("dummy1"), reinterpret_cast<void*>
//	(&dummy1)));
//	funcPtrIndex.insert(std::make_pair(String<MAXFUNCNAMELEN>("dummy2"), reinterpret_cast<void*>
//	(&dummy2)));
//
//	//reinterpret_cast<float(*)(float, int)>(funcPtrIndex["dummy1"])(3.14, 45);
//	//reinterpret_cast<void(*)(char)>(funcPtrIndex["dummy2"])('h');
//}

void FunctionManagementService::call(Message msg){
	uint8_t funcName[MAXFUNCNAMELEN];  // the function's name

	// initialize the function name array
	for (int i = 0; i < MAXFUNCNAMELEN; i++) {
		funcName[i] = 0;
	}

	// isolate the function's name from the incoming message
	for (int i = 0; i < MAXFUNCNAMELEN; i++) {
		uint8_t currByte = msg.readByte();
		if (currByte == 0x00) {
			funcName[i] = currByte;
			break;
		}
		funcName[i] = currByte;
	}

	// isolate the number of args (currently an unsigned 32-bit number, the standard doesn't
	// specify a maximum number)
	uint32_t numOfArgs = msg.readUint32();
	for (int i = 0; i < numOfArgs; i++) {
		// TODO: find a way to deduce the argument types as contained or store them somehow
		//  (finding a way to store them is better because it also solves the pointer casting
		//  problem)
	}

}
