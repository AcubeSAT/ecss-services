#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include <cmath>
#include <limits>
#include "etl/String.hpp"
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
	/**
	 * Appends the parameter as an ECSS value to an ECSS Message
	 */
	virtual void appendValueToMessage(Message& message) = 0;

	/**
	 * Given an ECSS message that contains this parameter as its first input, this loads the value from that parameter
	 */
	virtual void setValueFromMessage(Message& message) = 0;

	/**
	 * Converts the value of a parameter to a double.
	 *
	 * Some precision may be lost in the process. If the value is neither arithmetic nor enumerated,
	 * then usually 0 is returned.
	 */
	virtual double getValueAsDouble() = 0;

	/**
	 * Retrieves the value of a parameter as uint64_t for bit-precision operations.
	 *
	 * Some precision may be lost in the process. If the value is negative, then it is converted through int64_t.
	 * If the value is out of range, then the closest int64_t or uint64_t limit is returned. If the value is NaN,
	 * then 0 is returned.
	 */
	virtual uint64_t getValueAsUint64() = 0;

protected:
	template <typename T>
	static double toDouble(const T& value) {
		if constexpr (std::is_enum_v<T>) {
			return static_cast<double>(static_cast<std::underlying_type_t<T>>(value));
		} else if constexpr (std::is_arithmetic_v<T>) {
			return static_cast<double>(value);
		} else {
			return 0;
		}
	}

	template <typename T>
	static uint64_t toUint64(const T& value) {
		if constexpr (std::is_enum_v<T>) {
			return static_cast<uint64_t>(static_cast<std::underlying_type_t<T>>(value));
		} else if constexpr (std::is_floating_point_v<T>) {
			if (std::isnan(value)) {
				return 0;
			}
			if (value >= static_cast<T>(std::numeric_limits<uint64_t>::max())) {
				return std::numeric_limits<uint64_t>::max();
			}
			if (value <= static_cast<T>(std::numeric_limits<int64_t>::min())) {
				return static_cast<uint64_t>(std::numeric_limits<int64_t>::min());
			}
			if (value < static_cast<T>(0)) {
				return static_cast<uint64_t>(static_cast<int64_t>(value));
			}
			return static_cast<uint64_t>(value);
		} else if constexpr (std::is_arithmetic_v<T>) {
			return static_cast<uint64_t>(value);
		} else {
			return 0;
		}
	}
};

/**
 * Implementation of a parameter containing its value. See \ref ParameterBase for more information.
 * @tparam DataType The type of the Parameter value. This is the type used for transmission and reception
 * as per the PUS.
 */
template <typename DataType>
class Parameter : public ParameterBase {
protected:
	DataType currentValue;

public:
	constexpr explicit Parameter(DataType initialValue) : currentValue(initialValue) {}

	inline void setValue(DataType value) {
		currentValue = value;
	}

	inline DataType getValue() {
		return currentValue;
	}

	inline double getValueAsDouble() override {
		return toDouble(currentValue);
	}

	inline uint64_t getValueAsUint64() override {
		return toUint64(currentValue);
	}

	inline void setValueFromMessage(Message& message) override {
		currentValue = message.read<DataType>();
	};

	inline void appendValueToMessage(Message& message) override {
		message.append<DataType>(currentValue);
	};
};

#endif // ECSS_SERVICES_PARAMETER_HPP
