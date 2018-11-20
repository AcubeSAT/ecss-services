#include "Services/ParameterService.hpp"

#define DEMOMODE

#ifdef DEMOMODE

#include <ctime>
#include <cstdlib>

#endif

ParameterService::ParameterService() {
#ifdef DEMOMODE
	/**
	 * Initializes the parameter list with some dummy values for now.
	 */

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

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration **for the parameters specified in the carried IDs**.
	 * No sophisticated error checking for now, just whether the package is of the correct type
	 * (in which case it returns an empty message)
	 *
	 * @param paramId: a valid TC[20, 1] packet carrying the requested parameter ID
	 * @return A TM[20, 2] packet containing the parameter ID
	 * @todo Implement binary search for the lookup in order to be faster when the number of
	 * params inevitably rises
	 *
	 * NOTE: Everything apart from the setting data is uint16 (setting data are uint32 for now)
	 */

	Message reqParam(20, 2, Message::TM, 1);    // empty TM[20, 2] parameter report message

	if (paramIds.packetType == Message::TC && paramIds.serviceType == 20 &&
	    paramIds.messageType == 1) {

		uint16_t ids = paramIds.readUint16();        // first 16bits of the packet are # of IDs

		reqParam.appendUint16(ids);                  //include the number of contained IDs

		for (int i = 0; i < ids; i++) {

			uint16_t currId = paramIds.readUint16();      // current ID to be appended

			if (currId < CONFIGLENGTH) {  // check to prevent out-of-bounds access due to invalid id

				reqParam.appendUint16(currId);      // append it to the new packet
				reqParam.appendUint32(paramsList[currId].settingData);
				// right after that append the settings
			}
		}
	}

	return reqParam;
}

void ParameterService::setParamData(Message newParamValues) {

	/**
	 * This function receives a TC[20, 3] message and after checking whether its type is correct,
	 * replaces the setting specified in the settingData field of the parameter with the ID
	 * contained in the message with the value that the message carries. If the message type is
	 * not correct, the settings stay as they are.
	 *
	 * @param newParamValues: a valid TC[20, 3] message carrying parameter ID and replacement value
	 * @return None
	 * @todo Use pointers for changing and storing addresses to comply with the standard
	 */

	uint16_t reqParamId = newParamValues.readUint16();

	if (newParamValues.packetType == Message::TC && newParamValues.serviceType == 20 &&
		newParamValues.messageType == 1) {

		//TODO: Separate searching from rest of code
		for (int i = 0; i < CONFIGLENGTH; i++) {

			if (paramsList[i].paramId == reqParamId) {

				paramsList[i].settingData = newParamValues.readUint32();
				break;
			}
		}
	}
}
