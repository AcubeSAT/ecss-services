#include "Services/ParameterService.hpp"

#define DEMOMODE

#ifdef DEMOMODE

#include <ctime>

#endif

ParameterService::ParameterService() {
#ifdef DEMOMODE
	/**
	 * Initializes the parameter list with some dummy values for now.
	 * This normally will be initialized with actual values on boot.
	 */

	for (int i = 0; i < CONFIGLENGTH; i++) {

		paramsList[i].paramId = 0;
		paramsList[i].settingData = 0;
		paramsList[i].pfc = 1;
		paramsList[i].ptc = 1;
	}

	// Test code, setting up one of the parameter fields

	time_t currTime = time(nullptr);
	struct tm *today = localtime(&currTime);

	paramsList[2].paramId = 341;                   // random parameter ID
	paramsList[2].settingData = today->tm_min;   // the minute of the current hour
	paramsList[2].ptc = 3;                         // unsigned int
	paramsList[2].pfc = 14;                        // 32 bits
#endif
}

Message ParameterService::reportParameter(Message paramId) {

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration. No error checking for now, just whether
	 * the package is of the correct type (in which case it returns an empty message)
	 *
	 * @param paramId: a valid TC[20, 1] packet carrying the requested parameter ID
	 * @return A TM[20, 2] packet containing the parameter ID
	 * @todo Implement binary search for the lookup in order to be faster
	 */

	Message reqParam(20, 2, Message::TM, 1);    // empty TM[20, 2] parameter report message
	uint16_t reqParamId = paramId.readUint16(); // parameter ID must be accessed only once

	if (paramId.packetType == Message::TC && paramId.serviceType == 20 &&
	    paramId.messageType == 1) {

		for (int i = 0; i < CONFIGLENGTH; i++) {

			if (paramsList[i].paramId == reqParamId) {

				reqParam.appendUint16(paramsList[i].paramId);
				reqParam.appendUint32(paramsList[i].settingData);
				break;
			}
		}
	}

	return reqParam;
}

/*void ParameterService::setParamData(Message paramId) {

}*/

