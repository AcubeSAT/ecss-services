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
 *
 * @note The application ID was decided to be abolished as an identifier of the event-action
 * definition
 * @attention Every event action definition ID should be different, regardless of the application ID
 */
class EventActionService : public Service {
private:
	/**
	 * Event-action function status
	 */
	bool eventActionFunctionStatus = false;

public:
	inline static constexpr ServiceTypeNum ServiceType = 19;

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
		ApplicationProcessId applicationID = 0;
		inline static constexpr ApplicationProcessId MaxDefinitionID = 65535;
		EventDefinitionId eventDefinitionID = MaxDefinitionID;
		String<ECSSTCRequestStringSize> request = "";
		bool enabled = false;
		template <uint16_t Size>
		EventActionDefinition(ApplicationProcessId applicationID, EventDefinitionId eventDefinitionID, Message<Size>&
		message);
	};

	friend EventReportService;

	etl::multimap<uint16_t, EventActionDefinition, ECSSEventActionStructMapSize>
	    eventActionDefinitionMap;

	EventActionService() : eventActionFunctionStatus(true) {
		serviceType = ServiceType;
	}

	/**
	 * TC[19,1] add event-action definitions
	 */
	template <uint16_t Size>
	void addEventActionDefinitions(Message<Size>& message);

	/**
	 * TC[19,2] delete event-action definitions
	 */
	template <uint16_t Size>
	void deleteEventActionDefinitions(Message<Size>& message);

	/**
	 * TC[19,3] delete all event-action definitions
	 */
	template <uint16_t Size>
	void deleteAllEventActionDefinitions(const Message<Size>& message);

	/**
	 * TC[19,4] enable event-action definitions
	 */
	template <uint16_t Size>
	void enableEventActionDefinitions(Message<Size>& message);

	/**
	 * TC[19,5] disable event-action definitions
	 */
	template <uint16_t Size>
	void disableEventActionDefinitions(Message<Size>& message);

	/**
	 * TC[19,6] report the status of each event-action definition
	 */
	template <uint16_t Size>
	void requestEventActionDefinitionStatus(const Message<Size>& message);

	/**
	 * TM[19,7] event-action status report
	 */
	template <uint16_t Size>
	void eventActionStatusReport();

	/**
	 * TC[19,8] enable the event-action function
	 */
	template <uint16_t Size>
	void enableEventActionFunction(const Message<Size>& message);

	/**
	 * TC[19,9] disable the event-action function
	 */
	template <uint16_t Size>
	void disableEventActionFunction(const Message<Size>& message);

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
	 * Custom function that is called right after an event takes place, to initiate
	 * the execution of the action
	 */
	void executeAction(EventDefinitionId eventDefinitionID);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	template <uint16_t Size>
	void execute(Message<Size>& message);
};

#endif // ECSS_SERVICES_EVENTACTIONSERVICE_HPP
