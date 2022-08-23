#ifndef ECSS_SERVICES_LAZYPARAMETER_HPP
#define ECSS_SERVICES_LAZYPARAMETER_HPP

#include <etl/optional.h>
#include <functional>
#include "Parameter.hpp"

/**
 * A Lazy Parameter is a ParameterService parameter that does not keep a value in
 * memory, but calls an external function to fetch a new value whenever needed.
 *
 * The LazyParameter allows its users to call expensive value-fetching operators
 * ONLY when a value is requested. This prevents having to update a value in
 * memory every so often.
 *
 * This "lazy" fetching is useful when it is expensive (in terms of time, power
 * etc.) to get updated values, e.g. from peripherals or difficult calculations.
 *
 * @warning This class is NOT re-entrant. The developer will have to make sure
 * that only one thread has access to it at a time, otherwise undefined behaviour
 * will occur.
 *
 * @tparam DataType The data type of the parameter's value
 */
template <typename DataType>
class LazyParameter : public ParameterBase {
public:
	/**
	 * The type of the function that returns the current value of this parameter.
	 */
	using Getter = std::function<DataType()>;

	/**
	 * LazyParameter constructor without a getter function.
	 *
	 * When a getter function is not present, an error may be shown when fetching a value
	 * for the parameter. A @p fallback value will be returned in this case, if needed.
	 * @param fallback
	 */
	explicit LazyParameter(const DataType& fallback = 0) : fallback(fallback) {}

	/**
	 * LazyParameter constructor with a pre-defined getter function
	 * @param getter
	 * @param fallback
	 */
	explicit LazyParameter(const Getter& getter, const DataType& fallback = 0) : getter(getter), fallback(fallback) {}

	/**
	 * Set a getter function for this parameter. The getter function is called
	 * whenever a value for this parameter is requested.
	 */
	void setGetter(const Getter& _getter) {
		LazyParameter::getter = _getter;
	}

	/**
	 * Remove the getter function of this parameter.
	 */
	void unsetGetter() {
		getter.reset();
	}

	/**
	 * Get the current value of this parameter, if the getter is defined.
	 *
	 * @note This function may take some time to return a value, since it calls
	 * the "expensive" getter function.
	 */
	etl::optional<DataType> getValue() {
		if (getter) {
			return (*getter)();
		} else {
			return {};
		}
	}

	inline double getValueAsDouble() override {
		if constexpr (std::is_arithmetic_v<DataType>) {
			return static_cast<double>(getValue().value_or(fallback));
		} else {
			return 0;
		}
	}

	inline void appendValueToMessage(Message& message) override {
		if (getter) {
			message.append<DataType>((*getter)());
		} else {
			message.append<DataType>(fallback);
			ErrorHandler::reportError(message, ErrorHandler::ParameterValueMissing);
		}
	};

	inline void setValueFromMessage(Message& message) override {
		[[maybe_unused]] auto skippedBytes = message.read<DataType>();
		ErrorHandler::reportError(message, ErrorHandler::ParameterReadOnly);
	};
private:
	etl::optional<Getter> getter;
	DataType fallback;
};


#endif //ECSS_SERVICES_LAZYPARAMETER_HPP
