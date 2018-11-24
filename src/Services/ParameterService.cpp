#include "Services/ParameterService.hpp"

#define DEMOMODE

#ifdef DEMOMODE

#include <ctime>
#include <cstdlib>

#endif

ParameterService::ParameterService() {
#ifdef DEMOMODE
	/**
	 * Initializes the parameter list with some dummy values.
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
	 * containing the current configuration
	 * **for the parameters specified in the carried valid IDs**.
	 *
	 * No sophisticated error checking for now, just whether the package is of the correct type
	 * and whether the requested IDs are valid, ignoring the invalid ones.
	 *
	 * @param paramId: a valid TC[20, 1] packet carrying the requested parameter IDs
	 * @return A TM[20, 2] packet containing the valid parameter IDs and their settings.
	 * @return Empty TM[20, 2] packet on wrong type.
	 *
	 * @todo Generate failure notifs where needed when ST[01] is ready
	 *
	 * NOTES:
	 * Method for valid ID counting is a hack (clones the message and figures out the number
	 * separately, due to message access being non-random). Should be enough for now.
	 *
	 * Everything apart from the setting data is uint16 (setting data are uint32 for now)
	 */

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

	return reqParam;
}

void ParameterService::setParameterIds(Message newParamValues) {

	/**
	 * This function receives a TC[20, 3] message and after checking whether its type is correct,
	 * iterates over all contained parameter IDs and replaces the settings for each valid parameter,
	 * while ignoring all invalid IDs.
	 *
	 * @param newParamValues: a valid TC[20, 3] message carrying parameter ID and replacement value
	 * @return None
	 *
	 * @todo Generate failure notifications where needed (eg. when an invalid ID is encountered)
	 * @todo Use pointers for changing and storing addresses to comply with the standard
	 */

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

	idMsg.readPosition = 0;
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
