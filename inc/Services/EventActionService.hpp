#ifndef ECSS_SERVICES_EVENTACTIONSERVICE_HPP
#define ECSS_SERVICES_EVENTACTIONSERVICE_HPP

#define ECSS_EVENT_SERVICE_STRING_SIZE 64

#define ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE 256

#include "Service.hpp"
#include "MessageParser.hpp"
#include "etl/String.hpp"
#include <bitset>
#include <Services/EventReportService.hpp>

/**
 * Implementation of ST[19] event-action Service
 *
 * ECSS 8.19 && 6.19
 *
 *
 * @todo: (Possible) Use a etl::map for eventActionDefinitionArray
 * @todo: check if executeAction should accept applicationID too
 * @todo: Since there are multiple actions per event and in delete/enable/disable functions are
 * multiple instances are accessed, should I find a more efficient way to access them?
 * @todo: check if eventActionFunctionStatus should be private or not
 * @todo: check if eventAction array of definitions should be private or not
 * @todo: check size of eventActionDefinitionArray
 */
class EventActionService : public Service {
private:
	/**
	 * Custom function that is called right after an event takes place, to initiate
	 * the execution of the action
	 */
	void executeAction(uint16_t eventID);

public:
	struct EventActionDefinition {
		bool empty = true; // 1 means empty, 0 means full
		uint16_t applicationId = 0;
		uint16_t eventDefinitionID = 65535;
		String<64> request = "";
		bool enabled = false;
	};

	friend EventReportService;

	// If the size is changed maybe then indexOfAvailableSlots as well as the initiating loop in the
	// constructor should be changed from uint16_t
	EventActionDefinition eventActionDefinitionArray[ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE];

	EventActionService() {
		serviceType = 19;
		eventActionFunctionStatus = true;
	}

	/**
	 * Event-action function status
	 */
	bool eventActionFunctionStatus;

	/**
	 * TC[19,1] add event-action definitions
	 *
	 * Note: We have abolished multiple additions in one Telecommand packet. Only one
	 * event-action definition will be added per TC packet. That means there will be just an
	 * application ID, an event definition ID and the TC request.
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
	 * Setter for event-action function status
	 */
	void setEventActionFunctionStatus(bool status) {
		eventActionFunctionStatus = status;
	}
};

#endif //ECSS_SERVICES_EVENTACTIONSERVICE_HPP
