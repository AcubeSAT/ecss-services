#ifndef ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP
#define ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"

/**
 * Implementation of the ST[14] 'Real Time Forwarding Control Service' as defined in ECSS-E-ST-70-41C.
 *
 * @brief
 * The purpose of this Service is to control the forwarding of the stores telemetry to the ground stations. It
 * includes conditions for all the applications processes that are controlled by the Service, which determine whether
 * a message should be forwarded to the ground station through the corresponding virtual channel.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class RealTimeForwardingControlService {
public:
	inline static const uint8_t ServiceType = 14;

	enum MessageType : uint8_t {
		AddReportTypesToAppProcessConfiguration = 1,
		DeleteReportTypesFromAppProcessConfiguration = 2,
		ReportAppProcessConfigurationContent = 3,
		AppProcessConfigurationContentReport = 4,
		AddStructuresToHousekeepingConfiguration = 5,
		DeleteStructuresFromHousekeepingConfiguration = 6,
		ReportHousekeepingConfigurationContent = 7,
		HousekeepingConfigurationContentReport = 8,
		DeleteEventDefinitionsFromEventReportConfiguration = 13,
		AddEventDefinitionsToEventReportConfiguration = 14,
		ReportEventReportConfigurationContent = 15,
		EventReportConfigurationContentReport = 16,
	};

private:
	/**
	 * Contains the Application IDs, controlled by the Service.
	 */
	etl::vector<uint8_t, ECSSMaxControlledApplications> controlledApplications;

	/**
	 * Checks if the specified application process is controlled by the Service and returns true if it does.
	 */
	bool appIsControlled(uint8_t applicationId);

public:
	/**
	 * TC[14,1] 'Add report types to the application process forward control configuration'.
	 */
	void addReportTypesToAppProcessConfiguration(Message& request);

	/**
	 * TC[14,2] 'Delete report types from the application process forward control configuration'.
	 */
	void deleteReportTypesFromAppProcessConfiguration(Message& request);

	/**
	 * Receives a TC[14,3] 'Report the application process forward control configuration content' message and
	 * performs the necessary error checking.
	 */
	void reportAppProcessConfigurationContent(Message& request);

	/**
	 * Creates and stores a TM[14,4] 'Application process forward control configuration content report' message.
	 */
	void appProcessConfigurationContentReport();

	/**
	 * TC[14,5] 'Add structure identifiers to the housekeeping parameter report forward control configuration'.
	 */
	void addStructuresToHousekeepingConfiguration(Message& request);

	/**
	 * TC[14,6] 'Delete structure identifiers from the housekeeping parameter report forward control configuration'.
	 */
	void deleteStructuresFromHousekeepingConfiguration(Message& request);

	/**
	 * Receives a TC[14,7] 'Report the housekeeping parameter report forward control configuration content' message and
	 * performs the necessary error checking.
	 */
	void reportHousekeepingConfigurationContent(Message& request);

	/**
	 * Creates and stores a TM[14,8] 'Housekeeping parameter report forward control configuration content report'
	 * message.
	 */
	void housekeepingConfigurationContentReport();

	/**
	 * TC[14,13] 'Delete event definition identifiers from the event report blocking forward control configuration'.
	 */
	void deleteEventDefinitionsFromEventReportConfiguration(Message& request);

	/**
	 * TC[14,14] 'Add event definition identifiers to the event report blocking forward control configuration'.
	 */
	void addEventDefinitionsToEventReportConfiguration(Message& request);

	/**
	 * Receives a TC[14,15] 'Report the event report blocking forward control configuration content' message and
	 * performs the necessary error checking.
	 */
	void reportEventReportConfigurationContent(Message& request);

	/**
	 * Creates and stores a TM[14,16] 'Event report blocking forward control configuration content report' message.
	 */
	void eventReportConfigurationContentReport();

	/**
	 * It is responsible to call the suitable function that executes a TC packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif
