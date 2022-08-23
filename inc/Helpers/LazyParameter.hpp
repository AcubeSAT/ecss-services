#ifndef ECSS_SERVICES_LAZYPARAMETER_HPP
#define ECSS_SERVICES_LAZYPARAMETER_HPP

#include "Parameter.hpp"
#include <functional>
#include <etl/optional.h>

template <typename DataType>
class LazyParameter : public ParameterBase {
public:
	using Getter = std::function<DataType()>;

	explicit LazyParameter(const DataType& fallback = 0) : fallback(fallback) {}
	explicit LazyParameter(const Getter& getter, const DataType& fallback = 0) : getter(getter), fallback(fallback) {}

	void setGetter(const Getter& _getter) {
		LazyParameter::getter = _getter;
	}

	void unsetGetter() {
		getter.reset();
	}

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
