#ifndef ECSS_SERVICES_PARAMETERSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSERVICE_HPP

#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameter.hpp"
#include "etl/map.h"

// Number of stored parameters. MAX_PARAMS is just a dummy number for now.
#define MAX_PARAMS 5
// TODO: 1) Rework the parameter setting and report functions
// TODO: 2) Implement flags and use them above
// TODO: 3) Write more and better tests
// TODO: 4) Make sure that docs are up to date
// TODO: 5) Optimize stuff if possible

/**
 * Implementation of the ST[20] parameter management service,
 * as defined in ECSS-E-ST-70-41C
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

/**
 * Parameter manager - ST[20]
 * Holds the list with the parameters and provides functions
 * for parameter reporting and modification.
 *
 * The parameter list is stored in a map with the parameter IDs as keys and values
 * corresponding Parameter structs containing the PTC, PFC and the parameter's value.
 */


class ParameterService : public Service {
private:
	etl::map<ParamId, Parameter, MAX_PARAMS> paramsList;
	uint16_t numOfValidIds(Message idMsg); // count the valid ids in a given TC[20, 1]

public:
	/**
	 * Initializes the parameter list.
	 */
	ParameterService();

	/**
	 * Adds a new parameter. If the parameter has not been added (either because the map is full or because it already
	 * exists in it) then returns false.
	 */
	bool addNewParameter(uint8_t ptc, uint8_t pfc, uint32_t initialValue = 0, UpdatePtr ptr = nullptr);

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration
	 * **for the parameters specified in the carried valid IDs**.
	 *
	 * No sophisticated error checking for now, just whether the packet is of the correct type
	 * and whether the requested IDs are valid, ignoring the invalid ones.
	 * If the packet has an incorrect header, an InternalError::UnacceptablePacket is raised.
	 * If no IDs are correct, the returned message shall be empty.
	 *
	 * @param paramId: a valid TC[20, 1] packet carrying the requested parameter IDs
	 * @return None (messages are stored using storeMessage())
	 *
	 *
	 * NOTES:
	 * Method for valid ID counting is a hack (clones the message and figures out the number
	 * separately, due to message access being non-random). Should be enough for now.
	 *
	 * Everything apart from the setting data is uint16 (setting data are uint32 for now)
	 */
	void reportParameterIds(Message& paramIds);

	/**
	 * This function receives a TC[20, 3] message and after checking whether its type is correct,
	 * iterates over all contained parameter IDs and replaces the settings for each valid parameter,
	 * while ignoring all invalid IDs.
	 *
	 * @param newParamValues: a valid TC[20, 3] message carrying parameter ID and replacement value
	 * @return None
	 *
	 * @todo Use pointers for changing and storing addresses to comply with the standard
	 */
	void setParameterIds(Message& newParamValues);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param param Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_PARAMETERSERVICE_HPP
