#pragma once

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 *
 * The Application Process configuration. It's a map, storing a vector of report type definitions for each
 * pair of (applicationID, serviceType). It contains definitions, which indicate whether a telemetry message, produced
 * by a service, inside an application process (subsystem), should be forwarded to the ground station.
 */
class ApplicationProcessConfiguration {
public:
	/**
	 * Adds all report types of the specified application process definition, to the application process configuration.
	 */
	void addAllReportsOfApplication(ApplicationProcessId applicationID);

	/**
	 * Adds all report types of the specified service type, to the application process configuration.
	 */
	void addAllReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType);

	/**
	 * Counts the number of service types, stored for the specified application process.
	 */
	uint8_t countServicesOfApplication(ApplicationProcessId applicationID);

	/**
	 * Counts the number of report types, stored for the specified service type.
	 */
	uint8_t countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType);

	/**
	 * Checks whether the specified message type already exists in the application process and service
	 * type definition.
	 */
	bool reportExistsInAppProcessConfiguration(const Message& request, ApplicationProcessId applicationID,
		ServiceTypeNum serviceType,
		MessageTypeNum messageType);

	/**
	 * Performs the necessary error checking/logging for an application process ID. Also, skips the necessary
	 * bytes from the request message, in case of an invalid request.
	 *
	 * @return True: if the application is valid and passes all the necessary error checking.
	 */
	bool isApplicationOfAppProcessConfigValid(Message& request, ApplicationProcessId applicationID,
		uint8_t numOfServices,
		const etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications);

	/**
	 * Checks if all service types are allowed already, i.e. if the application process contains no service type
	 * definitions.
	 */
	bool areAllServiceTypesAllowed(const Message& request, ApplicationProcessId applicationID);

	/**
	 * Checks if the specified application process is controlled by the Service and returns true if it does.
	 */
	bool checkAppControlled(
		etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications, const
		Message& request,
		ApplicationProcessId applicationID);

	/**
	 * Checks if the maximum number of service type definitions per application process is reached.
	 */
	bool isMaxServiceTypesReached(const Message& request, ApplicationProcessId applicationID);

	/**
	 * Performs the necessary error checking/logging for a specific service type. Also, skips the necessary bytes
	 * from the request message, in case of an invalid request.
	 *
	 * @return True: if the service type is valid and passes all the necessary error checking.
	 */
	bool canServiceBeAdded(Message& request, ApplicationProcessId applicationID, uint8_t numOfMessages, ServiceTypeNum serviceType);

	/**
	 * Checks if the maximum number of report type definitions per service type definition is reached. Reports a MaxReportTypesReached
	 * error if it's true
	 */
	bool checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum
		serviceType);

	/**
	 * Checks if the maximum number of message types that can be contained inside a service type definition, is
	 * already reached and if the message type already exists. A MaxReportTypesReached or a AlreadyExistingReportType error
	 * will be generated if the checks don't pass.
	 *
	 * @return True: if the message type is valid and passes all the necessary error checking.
	 */
	bool canMessageBeAdded(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum serviceType,
		MessageTypeNum messageType);

	/**
	 * This function was created for the sole purpose of increasing readability where it's being used.
	 * @return true if the pair of applicationID and serviceType already exists in the definitions map, false otherwise
	 */
	bool isServiceExisting(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
		return definitions.find(std::make_pair(applicationID, serviceType)) != definitions.end();
	}

	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<MessageTypeNum, ECSSMaxReportTypeDefinitions> ReportTypeDefinitions;

	/**
	 * This is the key for the application process configuration map. It contains a pair with the applicationID and
	 * the serviceType.
	 */
	typedef std::pair<ApplicationProcessId, ServiceTypeNum> AppServiceKey;

	/**
	 * Map containing the report type definitions. Each application process has its own ID. The combination of the
	 * application ID and the service type is used as a key to provide access to the list of report type definitions.
	 *
	 * @note
	 * The report type definitions are the message types of each service. For example a message type for the
	 * 'ParameterStatisticsService' (ST04) is 'ParameterStatisticsService::MessageType::ParameterStatisticsReport'. The
	 * Real Time Forwarding Control Service (ST14) uses this map as a lookup table, to identify whether a requested
	 * triplet (app->service->message type) is allowed to be forwarded to the ground station via the corresponding virtual
	 * channel. The requested message type is only forwarded, if the requested application process ID and service type
	 * already exist in the map, and the requested report type is located in the vector of report types, which corresponds
	 * to the appID and service type.
	 *
	 * This will also be used by the StorageAndRetrievalService (ST15) as a look-up table, regarding which TMs are
	 * allowed to be stored.
	 */
	etl::map<AppServiceKey, ReportTypeDefinitions, ECSSMaxApplicationsServicesCombinations> definitions;

	ApplicationProcessConfiguration() = default;
};