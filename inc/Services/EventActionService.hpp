#ifndef ECSS_SERVICES_EVENTACTIONSERVICE_HPP
#define ECSS_SERVICES_EVENTACTIONSERVICE_HPP

#include "Service.hpp"
/**
 * Implementation of ST[19] event-action Service
 *
 * @todo: check if two enums are need for both event-action status and event-action definition
 * status
 */
class EventActionService : public Service {
/**
 * @todo: check if this should be private or not
 */
private:
	uint8_t eventActionStatus;
	uint8_t eventActionFunctionStatus;
public:
	EventActionService() {
		serviceType = 19;
		eventActionStatus = EventActionStatus::enabled;
		eventActionFunctionStatus = EventActionFunctionStatus::enabledFunction;
	}


	/**
	 * Event-action status
	 */
	enum EventActionStatus{
		disabled = 0,
		enabled = 1,
	};

	/**
	 * Event-action function status
	 */
	enum EventActionFunctionStatus {
		disabledFunction = 1,
		enabledFunction = 0,
	};
	/**
	 * TC[19,1] add event-action definitions
	 */
	void addEventActionDefinitions(Message message);

	/**
	 * TC[19,2] delete event-action definitions
	 */
	void deleteEventActionDefinitions(Message message);

	/**
	 * TC[19,3] delete all event-action definitions
	 */
	void deleteAllEventActionDefinitions(Message message);

	/**
	 * TC[19,4] enable event-action definitions
	 */
	void enableEventActionDefinitions(Message message);

	/**
	 * TC[19,5] disable event-action definitions
	 */
	void disableEventActionDefinitions(Message message);

	/**
	 * TC[19,6] report the status of each event-action definition
	 */
	void requestEventActionDefinitionStatus(Message message);

	/**
	 * TM[19,7] event-action status report
	 */
	void eventActionStatusReport();

	/**
	 * TC[19,8] enable the event-action function
	 */
	void enableEventActionFunction(Message message);

	/**
	 * TC[19,9] disable the event-actioni function
	 */
	void disableEventActionFunction(Message message);

	/**
	 * Setter for event-action status
	 */
	void setEventActionStatus(EventActionStatus e){
		eventActionStatus = e;
	}

	/**
	 * Setter for event-action function status
	 */
	void setEventActionFunctionStatus(EventActionFunctionStatus e){
		eventActionFunctionStatus = e;
	}
};

#endif //ECSS_SERVICES_EVENTACTIONSERVICE_HPP
