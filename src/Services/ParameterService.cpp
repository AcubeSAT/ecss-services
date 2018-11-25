#include "Services/ParameterService.hpp"

#define DEMOMODE

#ifdef DEMOMODE

#include <ctime>
#include <cstdlib>

#endif

ParameterService::ParameterService() {
#ifdef DEMOMODE
	// Test code, setting up some of the parameter fields

	time_t currTime = time(nullptr);
	struct tm *today = localtime(&currTime);

	paramsList[0].paramId = 0;                     // random parameter ID
	paramsList[0].settingData = today->tm_hour;    // the current hour
	paramsList[0].ptc = 3;                         // unsigned int
	paramsList[0].pfc = 14;                        // 32 bits

	paramsList[1].paramId = 1;                     // random parameter ID
	paramsList[1].settingData = today->tm_min;     // the current minute
	paramsList[1].ptc = 3;                         // unsigned int
	paramsList[1].pfc = 14;                        // 32 bits
#endif
}

Message ParameterService::reportParameterIds(Message paramIds) {
	Message reqParam(20, 2, Message::TM, 1);    // empty TM[20, 2] parameter report message

	if (paramIds.packetType == Message::TC && paramIds.serviceType == 20 &&
	    paramIds.messageType == 1) {
		uint16_t ids = paramIds.readUint16();
		reqParam.appendUint16(numOfValidIds(paramIds));   // include the number of valid IDs

		for (int i = 0; i < ids; i++) {
			uint16_t currId = paramIds.readUint16();      // current ID to be appended

			if (currId < CONFIGLENGTH) {  // check to prevent out-of-bounds access due to invalid id
				reqParam.appendUint16(currId);
				reqParam.appendUint32(paramsList[currId].settingData);
			} else {
								// generate failure of execution notification for ST[06]
				continue;       //ignore the invalid ID
			}
		}
	}

	storeMessage(reqParam);
	return reqParam;   // this has to stay for now because no other way for testing
}

void ParameterService::setParameterIds(Message newParamValues) {
	if (newParamValues.packetType == Message::TC && newParamValues.serviceType == 20 &&
	newParamValues.messageType == 3) {
		uint16_t ids = newParamValues.readUint16();  //get number of ID's

		for (int i = 0; i < ids; i++) {
			uint16_t currId = newParamValues.readUint16();

			if (currId < CONFIGLENGTH) {
				paramsList[currId].settingData = newParamValues.readUint32();
			} else {
								// generate failure of execution notification for ST[06]
				continue;       // ignore the invalid ID
			}
		}
	}
}

uint16_t ParameterService::numOfValidIds(Message idMsg) {
	idMsg.resetRead();
	// start reading from the beginning of the idMsg object
	// (original obj. will not be influenced if this is called by value)

	uint16_t ids = idMsg.readUint16();        // first 16bits of the packet are # of IDs
	uint16_t validIds = 0;

	for (int i = 0; i < ids; i++) {
		uint16_t currId = idMsg.readUint16();

		if (idMsg.messageType == 3) {
			idMsg.readUint32();   //skip the 32bit settings blocks, we need only the IDs
		}

		if (currId < CONFIGLENGTH) {
			validIds++;
		}
	}

	return validIds;
}
