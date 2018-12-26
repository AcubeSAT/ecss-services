#ifndef ECSS_SERVICES_EVENTACTIONSERVICE_HPP
#define ECSS_SERVICES_EVENTACTIONSERVICE_HPP

#include "Service.hpp"
#include "MessageParser.hpp"
#include "etl/String.hpp"
#include <bitset>

/**
 * Implementation of ST[19] event-action Service
 *
 * ECSS 8.19 && 6.19
 *
 * Note towards the reviewers: Please double-check the string sizes that I use, the string
 * initialization or rather the lack of it. Pay attention especially in parts of the code that I
 * use strings <3 .
 *
 * @todo: Do something with the applicationID.
 * @todo: check if eventActionFunctionStatus should be private or not
 * @todo: check if eventAction array of definitions should be private or not
 * @todo: check size of eventActionDefinitionArray
 */
class EventActionService : public Service {
private:
	uint8_t eventActionFunctionStatus; // Indicates if actions are enabled
	std::bitset<256> stateOfEventAction;
	struct EventActionDefinition {
		uint8_t empty = 1; // 1 means empty, 0 means full
		uint16_t applicationId = 0;
		uint16_t eventDefinitionID = 65535;
		String<ECSS_MAX_STRING_SIZE> request = "";
	};
	// If the size is changed maybe then indexOfAvailableSlots as well as the initiating loop in the
	// constructor should be changed from uint16_t
	EventActionDefinition eventActionDefinitionArray[256];
public:
	EventActionService() {
		serviceType = 19;
		eventActionFunctionStatus = enabledFunction;
		stateOfEventAction.set();
	}

	/**
	 * Event-action function status
	 */
	enum EventActionFunctionStatus {
		disabledFunction = 0,
		enabledFunction = 1,
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
	 * Custom function that is called right after an event takes place, to initiate
	 * the execution of the action
	 */
	void executeAction(Message &message);

	/**
	 * Setter for event-action function status
	 */
	void setEventActionFunctionStatus(EventActionFunctionStatus e) {
		eventActionFunctionStatus = e;
	}
};

#endif //ECSS_SERVICES_EVENTACTIONSERVICE_HPP
