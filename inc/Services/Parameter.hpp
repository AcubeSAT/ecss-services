#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/bitset.h"

// Number of binary flags in every parameter. Final number TBD.
#define NUM_OF_FLAGS 3

/**
 * Implementation of a Parameter field, as specified in ECSS-E-ST-70-41C.
 * Fully compliant with the standards requirements, while adding some small,
 * but useful extensions to its contents.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

/**
 * Useful type definitions
 *
 * @typedef ParamId: the unique ID of a parameter, used for searching
 * @typedef ValueType: the type of the parameter's value (changing types is WIP)
 * @typedef UpdatePtr: pointer to a void function, with a single ValueType* argument (return address)
 * @typedef Flags: container for the binary flags
 */
typedef uint16_t ParamIdType;
typedef uint32_t ValueType;
typedef void(*UpdatePtr)(ValueType*);
typedef etl::bitset<NUM_OF_FLAGS> Flags;

/**
 * Parameter class - Breakdown of fields
 *
 * @private ptc: The Packet field type code (PTC) as defined in ECSS-E-ST-70-41C, chapter 7.3.
 * @private pfc: The Packet field format code (PfC) as defined in the same standard
 * @private ptr: Pointer of the function that will update the parameter
 * @private currentValue: The current (as in last good) value of the parameter
 *
 * @todo: Find a way to store arbitrary types in currentValue
 *
 * Additional features (not included in standard):
 * @private flags: Various binary flags (number and meaning TBD).
 * @warning Current flag meanings (starting from LSB, big-endian):
 * Index 0: update with priority
 * Index 1: manual update available
 * Index 2: automatic update available
 *
 *
 * Methods:
 * @public Parameter(uint8_t newPtc, uint8_t newPfc, uint32_t initialValue = 0, UpdatePtr newPtr = nullptr):
 * Create a new Parameter object with newPtc PTC, newPfc PFC, initialValue as its starting value and newPtr
 * as its update function pointer. Arguments initialValue and newPtr are optional, and have default values of
 * 0 and nullptr respectively.
 *
 * @public setCurrentValue(): Changes the current value of the parameter
 * @public getCurrentValue(): Gets the current value of the parameter
 * @public getPTC(), getPFC(): Returns the PFC and PTC of the parameter
 */
class Parameter {
	uint8_t ptc;
	uint8_t pfc;
	UpdatePtr ptr;
	Flags flags;
	ValueType currentValue = 0;

	public:
		Parameter(uint8_t newPtc, uint8_t newPfc, uint32_t initialValue = 0, UpdatePtr newPtr = nullptr); // Ignore-MISRA

		void setCurrentValue(ValueType newVal);
		void setFlag(const char* flags);

		ValueType getCurrentValue();
		uint8_t getPTC();

		uint8_t getPFC();

};


#endif //ECSS_SERVICES_PARAMETER_HPP
