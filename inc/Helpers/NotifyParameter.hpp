#ifndef ECSS_SERVICES_NOTIFYPARAMETER_HPP
#define ECSS_SERVICES_NOTIFYPARAMETER_HPP

#include <etl/optional.h>
#include <functional>
#include "Parameter.hpp"

/**
 * A Notifying parameter will call a function whenever its value is written to.
 *
 * This is useful for updating the state of things when a parameter is changed,
 * for example to disable/enable peripherals, to make configuration changes etc.
 *
 * @warning Calling NotifyParameter::setValue will *not* call the notifier
 * function. You should use setValueLoudly for this purpose instead.
 *
 * @tparam DataType The data type of the parameter's value
 */
template <typename DataType>
class NotifyParameter : public Parameter<DataType> {
public:
	using Notifier = std::function<void(const DataType&)>;
	using Parent = Parameter<DataType>;

	/**
	 * Constructor without a notifier function. Nothing will then happen when the parameter is updated.
	 */
	explicit NotifyParameter(DataType initialValue) : Parent(initialValue) {}

	/**
	 * Constructor with a default notifier function.
	 */
	NotifyParameter(DataType initialValue, const Notifier& notifier) : Parent(initialValue), notifier(notifier) {}

	/**
	 * Same as Parameter::setValue(), but also calls the NotifyParameter::notifier function, if it
	 * exists.
	 */
	inline void setValueLoudly(DataType value) {
		Parent::setValue(value);

		if (notifier) {
			(*notifier)(Parent::currentValue);
		}
	}

	/**
	 * Call the notifier if it exists, without updating the value
	 */
	inline void notify() {
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

	/**
	 * Set the notifier function, to be called whenever the value of this parameter is updated.
	 *
	 * @note This function will be called even when a _parameter update_ command is received, but the
	 * new value is the same as the previous one. This is done so that there is an option to repair
	 * systems with a weird or unknown state.
	 * @param call Whether to also call the notifier function immediately, to ensure that a change is
	 * made.
	 */
	void setNotifier(const Notifier& _notifier, bool call=true) {
		notifier = _notifier;

		if (call) {
			_notifier(Parent::currentValue);
		}
	}

	/**
	 * Unset the notifier function, so that nothing is called when the value of this function is updated.
	 */
	void unsetNotifier() {
		notifier.reset();
	}

private:
	etl::optional<Notifier> notifier;
};


#endif //ECSS_SERVICES_NOTIFYPARAMETER_HPP
