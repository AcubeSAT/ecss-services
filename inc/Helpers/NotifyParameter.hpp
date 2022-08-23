#ifndef ECSS_SERVICES_NOTIFYPARAMETER_HPP
#define ECSS_SERVICES_NOTIFYPARAMETER_HPP

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
class NotifyParameter : public Parameter<DataType> {
public:
	using Notifier = std::function<void(const DataType&)>;
	using Parent = Parameter<DataType>;

	explicit NotifyParameter(DataType initialValue) : Parent(initialValue) {}

	NotifyParameter(DataType initialValue, const Notifier& notifier) : Parent(initialValue), notifier(notifier) {}

	inline void setValueLoudly(DataType value) {
		Parent::setValue(value);

		if (notifier) {
			(*notifier)(Parent::currentValue);
		}
	}

	inline void setValueFromMessage(Message& message) override {
		Parent::setValueFromMessage(message);

		if (notifier) {
			(*notifier)(Parent::currentValue);
		}
	}

	void setNotifier(const Notifier& _notifier) {
		notifier = _notifier;
	}

	void unsetNotifier() {
		notifier.reset();
	}

private:
	etl::optional<Notifier> notifier;
};


#endif //ECSS_SERVICES_NOTIFYPARAMETER_HPP
