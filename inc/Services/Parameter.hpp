#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/String.hpp"
#include "etl/vector.h"
#include "etl/map.h"
#include "Message.hpp"
#include "ECSS_Definitions.hpp"

/**
 * Implementation of a Parameter field, as specified in ECSS-E-ST-70-41C.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 *
 * @section Introduction
 * The Parameter class implements a way of storing and updating system parameters
 * of arbitrary size and type, while avoiding std::any and dynamic memory allocation.
 * It is split in two distinct parts:
 * 1) an abstract \ref ParameterBase class which provides a
 * common data type used to create any pointers to \ref Parameter objects, as well as
 * virtual functions for accessing the parameter's data part, and
 * 2) a templated \ref Parameter used to store any type-specific parameter information,
 * such as the actual data field where the parameter's value will be stored.
 *
 * @section Architecture Rationale
 * The ST[20] Parameter service is implemented with the need of arbitrary type storage
 * in mind, while avoiding any use of dynamic memory allocation, a requirement for use
 * in embedded systems. Since lack of Dynamic Memory Access precludes usage of stl::any
 * and the need for truly arbitrary (even for template-based objects like etl::string) type storage
 * would exclude from consideration constructs like etl::variant due to limitations on
 * the number of supported distinct types, a custom solution was needed.
 * Furthermore, the \ref ParameterService should provide ID-based access to parameters.
 */
class ParameterBase {
public:
	bool parameterIsActive = false;
	uint16_t sampleCounter = 0;

	virtual void appendValueToMessage(Message& message) = 0;
	virtual void setValueFromMessage(Message& message) = 0;
	virtual void appendSampleToMessage(Message& report, uint16_t index) = 0;

	void setParameterIsActive(bool paramIsActive) {
		this->parameterIsActive = paramIsActive;
	}
};

/**
 * Implementation of a parameter containing its value. See \ref ParameterBase for more information.
 * @tparam DataType The type of the Parameter value. This is the type used for transmission and reception
 * as per the PUS.
 */
template <typename DataType>
class Parameter : public ParameterBase {
private:
	DataType currentValue;

public:
	explicit Parameter(DataType initialValue) : currentValue(initialValue) {}

	inline void setValue(DataType value) {
		currentValue = value;
	}

	/**
	 * Vector containing all the samples gathered for this specific parameter.
	 */
	etl::vector<DataType, ECSS_MAX_HOUSEKEEPING_PARAMETER_SAMPLES> samples;

	/**
	 * Helper function that appends parameter samples to a report message.
	 *
	 * @param report Target report message to append the sampled value
	 * @param index Position of the sampled value in the samples-vector.
	 */
	inline void appendSampleToMessage(Message& report, uint16_t index) override {
		DataType sampleValue = samples[index];
		report.append(sampleValue);
	}

	DataType getValue() {
		return currentValue;
	}

	/**
	 * Given an ECSS message that contains this parameter as its first input, this loads the value from that paremeter
	 */
	inline void setValueFromMessage(Message& message) override {
		currentValue = message.read<DataType>();
	};

	/**
	 * Appends the parameter as an ECSS value to an ECSS Message
	 */
	inline void appendValueToMessage(Message& message) override {
		message.append<DataType>(currentValue);
	};
};

#endif // ECSS_SERVICES_PARAMETER_HPP
