#include "Services/ParameterService.hpp"
#include <ctime>

ParameterService::ParameterService() {

	/**
	 * Initializes the parameter list with some dummy values for now.
	 * This normally will be initialized with actual values on boot.
	 */

	for (int i = 0; i < 5; i++) {

		paramsList[i].paramId = 0;
		paramsList[i].settingData = 0;
		paramsList[i].pfc = 1;
		paramsList[i].ptc = 1;
	}

	//Test code, setting up one of the parameter fields

	time_t currTime = time(NULL);
	struct tm* today = localtime(&currTime);

	paramsList[2].paramId = 10;  //random parameter ID
	paramsList[2].settingData = today -> tm_year;   //the current year
	paramsList[2].ptc = 3;  //unsigned int
	paramsList[2].pfc = 14;  //32 bits
}

Message ParameterService::reportParameter(Message paramId) {

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration.
	 *
	 * @param Message paramSpecifier: a valid TC[20, 1] packet carrying the requested parameter ID
	 */

	Message reqParam(20, 2, Message::TM, 1);    //empty TM[20, 2] parameter report message
	//TODO: Try to have the parameter list always sorted so binary search can be a thing

	if (paramId.packetType == Message::TC) {

		if (paramId.serviceType == 20 && paramId.messageType == 1) {

			//if TC is of the wrong type return an empty message for now
			for (int i = 0; i < CONFIGLENGTH; i++) {    //5 is a dummy, as always

				if (paramsList[i].paramId == *paramId.data) {

					reqParam.appendHalfword(paramsList[i].paramId);
					reqParam.appendWord(paramsList[i].settingData);
				}
			}
		}
	}

	return reqParam;
}

void ParameterService::setParamData(Message paramId) {

}