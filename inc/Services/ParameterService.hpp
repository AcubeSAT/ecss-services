#ifndef ECSS_SERVICES_PARAMETERSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSERVICE_HPP

#include "Service.hpp"
// #include "Services/RequestVerificationService.hpp"

#define CONFIGLENGTH 5

/**
 * Implementation of the ST[20] parameter management service,
 * as defined in ECSS-E-ST-70-41C
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

/**
 * Generic parameter structure
 * PTC and PFC for each parameter shall be specified as in
 * ECSS-E-ST-70-41C, chapter 7.3
 */
struct Parameter {
	uint8_t ptc;            // Packet field type code (PTC)
	uint8_t pfc;            // Packet field format code (PFC)
	uint16_t paramId;       // Unique ID of the parameter

	uint32_t settingData;
	// Actual data defining the operation of a peripheral or subsystem.
	// Peripheral-dependent normally (void* maybe?) (it's a memory address according to spec).
	// Dummy int for now.
};

/**
 * Parameter manager - ST[20]
 * Holds the list with the parameters and provides functions
 * for parameter reporting and modification.
 *
 * @todo Ensure that the parameter list is sorted by ID
 * @todo Convert reportParameterIds() to void when storeMessage() stores and doesn't print
 */

class ParameterService : public Service {
private:
	Parameter paramsList[CONFIGLENGTH];
	// CONFIGLENGTH is just a dummy number for now, this should be statically set
	static uint16_t numOfValidIds(Message idMsg);  //count the valid ids in a given TC[20, 1]

public:
	/**
	 * Initializes the parameter list with some dummy values for now.
	 */
	ParameterService();

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
	Message reportParameterIds(Message paramIds);

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
	void setParameterIds(Message newParamValues);

};

#endif //ECSS_SERVICES_PARAMETERSERVICE_HPP
