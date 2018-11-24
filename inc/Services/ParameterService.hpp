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
 * Parameter manager
 * Holds the list with the parameters and provides functions
 * for parameter reporting and modification.
 *
 * @todo Ensure that the parameter list is sorted by ID
 */

class ParameterService : public Service {
private:
	Parameter paramsList[CONFIGLENGTH];
	// CONFIGLENGTH is just a dummy number for now, this should be statically set
	static uint16_t numOfValidIds(Message idMsg);  //count the valid ids in a given TC[20, 1]

public:
	ParameterService();

	Message reportParameterIds(Message paramIds);

	void setParameterIds(Message newParamValues);

};

#endif //ECSS_SERVICES_PARAMETERSERVICE_HPP
