#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/String.hpp"
#include "ECSS_Definitions.hpp"
/**
 * Implementation of a Parameter field, as specified in ECSS-E-ST-70-41C.
 * Fully compliant with the standard's requirements.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 * @author Athanasios Theocharis <athatheo@csd.auth.gr>
 * 
 * 
 * The Parameter class implements a way of storing and updating system parameters
 * of arbitrary size and type, while avoiding std::any and dynamic memory allocation.
 * 
 * It is split in two parts: an abstract ParameterBase class which contains the setter,
 * and a 
 */

/**
 * Useful type definitions
 * (DEPRECATED - MARK FOR REMOVAL)
 * @typedef ParamId: the unique ID of a parameter, used for searching
 */
typedef uint16_t ParamId;

/*
 * MILLION DOLLAR QUESTIONS:
 * setCurrentValue is templated. Since Parameter (a template class) inherits ParameterBase
 * (a class containing a template member), does a specialization of Parameter also specialize
 * setCurrentValue? If not, we have a problem, since Parameter won't necessarily specialize
 * setCurrentValue with the correct type => our setter is *not* typesafe.
 *
 * Answer: NO! After a discussion on ##C++-general@Freenode, it turns out that this specialization
 * does not happen! What this means is that, while a Parameter can be specialized as e.g. int, there
 * is no way to prevent the setter from being called with a non-int (e.g. string or float) argument,
 * resulting in an attempt to write data inside the Parameter that have a different type from what the
 * template is carrying.
 *
 * Proof of concept:
 *      Parameter<int> ircTest = Parameter<int>(1337);
 *		ircTest.setCurrentValue("Hello this is a problem speaking");
 *		auto s = ircTest.getValueAsString();
 *
 * This snippet will overwrite the 4 bytes of the Parameter field with garbage and most importantly,
 * it will do so silently.
 *
 *
 */

class ParameterBase {
public:
	virtual String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() = 0;
	virtual void setValueFromMessage(Message message) = 0;
};


template <typename DataType>
class Parameter : public ParameterBase {
	DataType currentValue;
	void (* updateFunction)(DataType*);

public:
	Parameter(DataType initialValue, void (* updateFunction)(DataType*) = nullptr) {
		this.updateFunction = updateFunction;

		if (this.updateFunction != nullptr) {
			(*updateFunction)(&currentValue);
		}
		else {
			currentValue = initialValue;
		}
	}

	void setValueFromMessage(Message message) override {
		// TODO: implement setValueForMessage
	}

	String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() override {
		// TODO: implement getValueAsString()
		return String<ECSS_ST_20_MAX_STRING_LENGTH>("DUMMY STRING");
	}
};

// class ParameterBase {
// public:

// 	virtual String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() = 0;
// 	virtual void setValueFromMessage(Message message) = 0;

// 	template <typename ValueType>
// 	void setCurrentValue(ValueType newVal) {
// 		*reinterpret_cast<ValueType*>(valuePtr) = newVal;
// 	}
// };

// template <typename ValueType>
// class Parameter : public ParameterBase {
// 	void (* ptr)(ValueType*);
// 	ValueType currentValue;

// public:
// 	Parameter(ValueType initialValue = 0, void(* newPtr)(ValueType*) = nullptr) {
// 		ptr = newPtr;
// 		sizeInBytes = sizeof(initialValue);
// 		// previously null valuePtr now points to the currentValue field	
// 		valuePtr = static_cast<void*>(&currentValue);

// 		if (ptr != nullptr) {
// 			(*ptr)(&currentValue);  // call the update function for the initial value
// 		} else {
// 			currentValue = initialValue;
// 		}
// 	}

// 	String<ECSS_ST_20_MAX_STRING_LENGTH> getValueAsString() override {
// 		String<ECSS_ST_20_MAX_STRING_LENGTH> contents(reinterpret_cast<uint8_t*>(&currentValue), sizeInBytes);
// 		return contents;
// 	}
// };


#endif //ECSS_SERVICES_PARAMETER_HPP
