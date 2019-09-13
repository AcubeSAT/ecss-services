#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/bitset.h"
#include "etl/String.hpp"

// Number of binary flags in every parameter. Final number TBD.
#define NUM_OF_FLAGS 3
// Maximum etl::string output length in bytes
#define MAX_STRING_LENGTH 30
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
 * @typedef ParamIdType: the unique ID of a parameter, used for searching
 * @typedef Flags: container for the binary flags
 */
typedef uint16_t ParamIdType;
typedef etl::bitset<NUM_OF_FLAGS> Flags;
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

    void setValueAsString(const String<MAX_STRING_LENGTH> & input);

    uint8_t getSizeInBytes() const {
        return sizeInBytes;
    }

    void* ptr() { return valuePtr; }

    void setPtr(void *valuePtr) {
        this->valuePtr = valuePtr;
    }

    virtual String<MAX_STRING_LENGTH> getValueAsString() {
        String<MAX_STRING_LENGTH> contents(reinterpret_cast<uint8_t *>(valuePtr), sizeInBytes);
        return contents;
    }

    template <class T>
    static T change_endian_double(T in)
    {
        char* const p = reinterpret_cast<char*>(&in);
        for (size_t i = 0; i < sizeof(T) / 2; ++i)
            std::swap(p[i], p[sizeof(T) - i - 1]);
        return in;
    }

    virtual ~ParameterBase() = default;
};

class StringParameter : public ParameterBase {
    String<10> value;
public:
    StringParameter() : value("bla") {
        sizeInBytes = 10;
    }

    String<MAX_STRING_LENGTH> getValueAsString() override {
        for (int i = value.size(); i < 10; i++) {
            // Null bytes for the data
            value[i] = '\0';
        }
        return String<MAX_STRING_LENGTH>(reinterpret_cast<const unsigned char*>(value.c_str()), sizeInBytes);
    }

    void setCurrentValue(String<10> newValue) {
        value = newValue;
    }
};

template <typename ValueType>
class Parameter : public ParameterBase {
	void (* ptr)(ValueType*);
	ValueType currentValue;

public:
    Parameter() noexcept {
        sizeInBytes = sizeof(ValueType);
        valuePtr = static_cast<void*>(&currentValue);
    }

	Parameter(uint8_t newPtc, uint8_t newPfc, ValueType initialValue = 0, void(* newPtr)(ValueType*) = nullptr) noexcept {
		ptc = newPtc;
		pfc = newPfc;
		ptr = newPtr;
		sizeInBytes = sizeof(ValueType);
		valuePtr = static_cast<void*>(&currentValue);
		// see Parameter.hpp for explanation on flags
		// by default: no update priority, manual and automatic update available

		if (ptr != nullptr) {
			(*ptr)(&currentValue);  // call the update function for the initial value
		} else {
			currentValue = initialValue;
		}
	}

	/**
	 * Get the current value of this parameter
	 * @return The parameter
	 */
	ValueType getValue() const {
        return *static_cast<ValueType*>(valuePtr);
	}

	/**
	 * Set the current value of this parameter
	 * @param newVal
	 */
    void setCurrentValue(ValueType newVal) {
        *reinterpret_cast<ValueType*>(valuePtr) = newVal;
    }
};


#endif //ECSS_SERVICES_PARAMETER_HPP
