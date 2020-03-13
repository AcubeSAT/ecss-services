#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

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
 */
typedef uint16_t ParamId;

/**
 * Parameter class - Breakdown of fields
 *
 * @private ptr: Pointer of the function that will update the parameter
 * @private currentValue: The current (as in last good) value of the parameter
 *
 * @todo: Find a way to store arbitrary types in currentValue
 *
 *
 * Methods:
 * @public Parameter(uint32_t initialValue = 0, UpdatePtr newPtr = nullptr):
 * Create a new Parameter object with initialValue as its starting value and newPtr
 * as its update function pointer. Arguments initialValue and newPtr are optional, and have default values of
 * 0 and nullptr respectively.
 *
 * @public setCurrentValue(): Changes the current value of the parameter
 * @public getCurrentValue(): Gets the current value of the parameter
 */

class ParameterBase {
protected:
	uint8_t sizeInBytes;
	void* valuePtr;
public:

	virtual String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() = 0;

	template <typename ValueType>
	void setCurrentValue(ValueType newVal) {
		// set the value only if the parameter can be updated manually
		*reinterpret_cast<ValueType*>(valuePtr) = newVal;
	}
};

template <typename ValueType>
class Parameter : public ParameterBase {
	void (* ptr)(ValueType*);
	ValueType currentValue;

public:
	Parameter(ValueType initialValue = 0, void(* newPtr)(ValueType*) = nullptr) {
		ptr = newPtr;
		sizeInBytes = sizeof(initialValue);
		valuePtr = static_cast<void*>(&currentValue);

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
