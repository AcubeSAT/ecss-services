#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/bitset.h"

// Number of binary flags in every parameter.
#define NUM_OF_FLAGS 5

/**
 * Generic parameter structure
 * PTC and PFC for each parameter shall be specified as in
 * ECSS-E-ST-70-41C, chapter 7.3
 */

typedef uint16_t ParamId;             // parameter IDs are given sequentially
typedef void(*UpdatePtr)(uint32_t*);  // pointer to the update function of this parameter
// (argument is a pointer to the variable where the value will be returned, in
// this case currentValue)

class Parameter {
	uint8_t ptc = 0;                                        // Packet field type code (PTC)
	uint8_t pfc = 0;                                        // Packet field format code (PFC)
	UpdatePtr ptr = nullptr;                                // Function pointer used for updates
	uint32_t currentValue = 0;                              // Last good value of the parameter
	etl::bitset<NUM_OF_FLAGS> flags = {false};
	// Various flags (TBD which. Ideas: update with priority, do not poll, etc.)

	public:
		Parameter(uint8_t new_ptc, uint8_t new_pfc, UpdatePtr new_ptr) {
			ptc = new_ptc;
			pfc = new_pfc;
			ptr = new_ptr;

			(*ptr)(&currentValue);  // call the update function for the initial value
		}
};


#endif //ECSS_SERVICES_PARAMETER_HPP
