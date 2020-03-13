#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/bitset.h"
#include "etl/String.hpp"
#include "ECSS_Definitions.hpp"

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
 * @typedef Flags: container for the binary flags
 */
typedef uint16_t ParamId;
typedef etl::bitset<ECSS_ST_20_NUMBER_OF_FLAGS> Flags;
typedef enum {STRING = 0,
	INT32 = 1,
	} TypesList;

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

class ParameterBase {
protected:
	uint8_t ptc;
	uint8_t pfc;
	uint8_t sizeInBytes;
	void* valuePtr;
	Flags flags;
public:
	uint8_t getPTC();

	void setFlags(const char* flags);

	uint8_t getPFC();

	virtual String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() = 0;

	template <typename ValueType>
	void setCurrentValue(ValueType newVal) {
		// set the value only if the parameter can be updated manually
		if (flags[1]) {
			*reinterpret_cast<ValueType*>(valuePtr) = newVal;
		}
	}
};

template <typename ValueType>
class Parameter : public ParameterBase {
	void (* ptr)(ValueType*);
	ValueType currentValue;

public:
	Parameter(uint8_t newPtc, uint8_t newPfc, ValueType initialValue = 0, void(* newPtr)(ValueType*) = nullptr) {
		ptc = newPtc;
		pfc = newPfc;
		ptr = newPtr;
		sizeInBytes = sizeof(initialValue);
		valuePtr = static_cast<void*>(&currentValue);
		// see Parameter.hpp for explanation on flags
		// by default: no update priority, manual and automatic update available

		if (ptr != nullptr) {
			(*ptr)(&currentValue);  // call the update function for the initial value
		} else {
			currentValue = initialValue;
		}
	}

	String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() override {
		String<ECSS_ST_20_MAX_STRING_LENGTH> contents(reinterpret_cast<uint8_t*>(&currentValue), sizeInBytes);
		return contents;
	}
};


#endif //ECSS_SERVICES_PARAMETER_HPP
