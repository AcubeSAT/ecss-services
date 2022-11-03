#ifndef ECSS_SERVICES_EVENTACTIONSERVICE_HPP
#define ECSS_SERVICES_EVENTACTIONSERVICE_HPP

#include "Service.hpp"
#include "Services/EventReportService.hpp"
#include "etl/String.hpp"
#include "etl/multimap.h"

/**
 * Implementation of ST[19] event-action Service
 *
 * ECSS 8.19 && 6.19
 *
 * @ingroup Services
 * @note Make sure to check the note in the addEventActionDefinition()
 * @note A third variable was added, the eventActionDefinitionID. This was added for the purpose of identifying
 * various eventActionDefinitions that correspond to the same eventDefinitionID. The goal is to have multiple actions
 * be executed when one event takes place. This defies the standard.
 * @note The application ID was decided to be abolished as an identifier of the event-action
 * definition
 * @attention Every event action definition ID should be different, regardless of the application ID
 *
 * @todo Since there are multiple actions per event and in delete/enable/disable functions are
 * multiple instances are accessed, should I find a more efficient way to access them?
 * @todo check if eventActionFunctionStatus should be private or not
 * @todo check if eventAction map of definitions should be private or not
 */
class EventActionService : public Service {
private:
	/**
	 * Event-action function status
	 */
	bool eventActionFunctionStatus = true;

	/**
	 * Custom function that is called right after an event takes place, to initiate
	 * the execution of the action
	 */
	void executeAction(uint16_t eventID);

public:
	inline static const uint8_t ServiceType = 19;

	enum MessageType : uint8_t {
		AddEventAction = 1,
		DeleteEventAction = 2,
		DeleteAllEventAction = 3,
		EnableEventAction = 4,
		DisableEventAction = 5,
		ReportStatusOfEachEventAction = 6,
		EventActionStatusReport = 7,
		EnableEventActionFunction = 8,
		DisableEventActionFunction = 9
	};

	struct EventActionDefinition {
		// TODO: APID = 0 is the Ground Station APID. This should be changed
		uint16_t applicationId = ApplicationId;
		uint16_t eventDefinitionID = 65535;   // The ID of the event that might take place
		uint16_t eventActionDefinitionID = 0; // The ID of the event-action
		String<64> request = "";
		bool enabled = false;
	};

	friend EventReportService;

	etl::multimap<uint16_t, EventActionDefinition, ECSSEventActionStructMapSize>
	    eventActionDefinitionMap;

	EventActionService() {
		serviceType = ServiceType;
	}

	/**
	 * TC[19,1] add event-action definitions
	 *
	 * Note: We have abolished multiple additions in one Telecommand packet. Only one
	 * event-action definition will be added per TC packet. That means there will be just an
	 * application ID, an event definition ID and the TC request.
	 */
	void addEventActionDefinitions(Message& message);

	/**
	 * TC[19,2] delete event-action definitions
	 */
	void deleteEventActionDefinitions(Message& message);

	/**
	 * TC[19,3] delete all event-action definitions
	 */
	void deleteAllEventActionDefinitions(Message& message);

	/**
	 * TC[19,4] enable event-action definitions
	 */
	void enableEventActionDefinitions(Message& message);

	/**
	 * TC[19,5] disable event-action definitions
	 */
	void disableEventActionDefinitions(Message& message);

	/**
	 * TC[19,6] report the status of each event-action definition
	 */
	void requestEventActionDefinitionStatus(Message& message);

	/**
	 * TM[19,7] event-action status report
	 */
	void eventActionStatusReport();

	/**
	 * TC[19,8] enable the event-action function
	 */
	void enableEventActionFunction(Message& message);

	/**
	 * TC[19,9] disable the event-actioni function
	 */
	void disableEventActionFunction(Message& message);

	/**
	 * Setter for event-action function status
	 */
	void setEventActionFunctionStatus(bool status) {
		eventActionFunctionStatus = status;
	}

	/**
	 * Getter for event-action function status
	 * @return eventActionFunctionStatus
	 */
	bool getEventActionFunctionStatus() const {
		return eventActionFunctionStatus;
	}

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_EVENTACTIONSERVICE_HPP
