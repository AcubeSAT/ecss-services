#ifndef ECSS_SERVICES_PARAMETERSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSERVICE_HPP

#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameter.hpp"
#include "etl/map.h"
#include "etl/vector.h"

// Number of stored parameters. MAX_PARAMS is just a dummy number for now.
#define MAX_PARAMS 5

class HousekeepingService; // forward declaration

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
 * corresponding Parameter classes containing the PTC, PFC and the parameter's value.
 */


class ParameterService : public Service {
private:
	etl::map<ParamId, Parameter, MAX_PARAMS> paramsList;
	friend class HousekeepingService;

public:
	/**
	 * @brief Initializes the parameter list.
	 */
	ParameterService();

	/**
	 * @brief Adds a new parameter. Returns false if the parameter has not been added
	 * (either because the map is full or because it already exists in it).
	 * @param id: the desired ID for this parameter
	 * @param param: the parameter field to be included
	 * @param flags: the flags to be set for this field (see Parameter.hpp)
	 */
	bool addNewParameter(uint16_t id, Parameter param, const char* flags = "110");

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration
	 * **for the parameters specified in the carried valid IDs**.
	 *
	 * The packet is checked for errors in service and message type, as well as for the
	 * validity of the IDs contained. For every invalid ID an ExecutionStartErrorType::UnknownExecutionStartError
	 * is raised.
	 * If the packet has an incorrect header and service type, an InternalError::UnacceptableMessage is raised.
	 * If no IDs are correct, the returned message shall be empty.
	 *
	 * @param paramId: a TC[20, 1] packet carrying the requested parameter IDs
	 * @return None (messages are stored using storeMessage())
	 *
	 * Everything apart from the setting data is uint16 (setting data are uint32 for now)
	 */
	void reportParameterIds(Message& paramIds);

	/**
	 * This function receives a TC[20, 3] message and after checking whether its type is correct,
	 * iterates over all contained parameter IDs and replaces the settings for each valid parameter,
	 * while ignoring all invalid IDs. If the manual update flag is not set, the parameter's value should
	 * not change.
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
