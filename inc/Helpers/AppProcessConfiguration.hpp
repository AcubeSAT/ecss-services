#pragma once

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * The Application Process configuration. It's a map, storing a vector of report type definitions for each
 * pair of (applicationID, serviceType). It contains definitions, which indicate whether a telemetry message, produced
 * by a service, inside an application process (subsystem), should be forwarded to the ground station (ST[14]) or
 * stored into a packet store (ST[15]).
 *
 * @note
 * Functions named `checkXYZ` report errors through the ErrorHandler, as documented per function.
 * Functions named `isXYZ`, `areXYZ` or `countXYZ` are pure queries and never report errors.
 */
class ApplicationProcessConfiguration {
public:
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

	/**
	 * Adds all report types of the specified application process definition, to the application process configuration.
	 * May report errors through checkMessageCanBeAdded for report types that cannot be added.
	 */
	void addAllReportsOfApplication(const Message& message, ApplicationProcessId applicationID);

	/**
	 * Adds all report types of the specified service type, to the application process configuration.
	 * May report errors through checkMessageCanBeAdded for report types that cannot be added.
	 */
	void addAllReportsOfService(const Message& message, ApplicationProcessId applicationID, ServiceTypeNum serviceType);

	/**
	 * Adds the specified report type to the application process configuration. The caller is responsible for
	 * performing the necessary error checking first, via checkMessageCanBeAdded.
	 */
	void addReport(ApplicationProcessId applicationID, ServiceTypeNum serviceType, MessageTypeNum messageType);

	/**
	 * Counts the number of service types, stored for the specified application process.
	 */
	uint8_t countServicesOfApplication(ApplicationProcessId applicationID) const;

	/**
	 * Counts the number of report types, stored for the specified service type.
	 */
	uint8_t countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const;

	/**
	 * Performs the necessary error checking/logging for an application process ID of an 'add report types' request.
	 * Also, skips the necessary bytes from the request message, in case of an invalid request.
	 * Reports a NotControlledApplication or an AllServiceTypesAlreadyAllowed error for invalid requests.
	 *
	 * @return True: if the application is valid and passes all the necessary error checking.
	 */
	bool checkApplicationOfAppProcessConfigValid(Message& request, ApplicationProcessId applicationID,
	    uint8_t numOfServices,
	    const etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications);

	/**
	 * Checks if all service types are effectively allowed already, i.e. if the maximum number of service type
	 * definitions per application process has been reached. Reports an AllServiceTypesAlreadyAllowed error if so.
	 *
	 * @return True: if the maximum number of service type definitions has already been reached.
	 */
	bool checkAllServiceTypesAllowed(const Message& request, ApplicationProcessId applicationID);

	/**
	 * Checks if the specified application process is controlled by the Service.
	 * Reports a NotControlledApplication error if it is not.
	 *
	 * @return True: if the application process is controlled by the Service.
	 */
	bool checkAppControlled(
	    const etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications,
	    const Message& request, ApplicationProcessId applicationID);

	/**
	 * Checks if the maximum number of service type definitions per application process is reached.
	 * Reports a MaxServiceTypesReached error if it is.
	 *
	 * @return True: if the maximum number of service type definitions has already been reached.
	 */
	bool checkMaxServiceTypesReached(const Message& request, ApplicationProcessId applicationID);

	/**
	 * Performs the necessary error checking/logging for a specific service type of an 'add report types' request.
	 * Also, skips the necessary bytes from the request message, in case of an invalid request.
	 * Reports a NonExistentServiceTypeDefinition error if the service type is unknown to AllReportTypes, or a
	 * MaxServiceTypesReached error if no new service type definition can be added.
	 *
	 * @return True: if the service type is valid and passes all the necessary error checking.
	 */
	bool checkServiceCanBeAdded(Message& request, ApplicationProcessId applicationID, uint8_t numOfMessages,
	    ServiceTypeNum serviceType);

	/**
	 * Checks if the maximum number of report type definitions per service type definition is reached.
	 * Reports a MaxReportTypesReached error if it is.
	 *
	 * @return True: if the maximum number of report type definitions has already been reached.
	 */
	bool checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID,
	    ServiceTypeNum serviceType);

	/**
	 * Checks if the specified message type can be added to the specified application process and service type
	 * definition. Reports a MaxReportTypesReached or an AlreadyExistingReportType error if the checks don't pass.
	 *
	 * @return True: if the message type is valid and passes all the necessary error checking.
	 */
	bool checkMessageCanBeAdded(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum serviceType,
	    MessageTypeNum messageType);

	/**
	 * Checks whether the specified message type already exists in the application process and service
	 * type definition. Reports an AlreadyExistingReportType error if it does.
	 *
	 * @return True: if the message type already exists in the definition.
	 */
	bool checkAlreadyExistingReport(const Message& request, ApplicationProcessId applicationID,
	    ServiceTypeNum serviceType,
	    MessageTypeNum messageType);

	/**
	 * Checks whether the requested application process is present in the application process configuration.
	 * Reports a NonExistentApplicationProcess error if it is not, skipping the necessary amount of bytes in the
	 * request.
	 *
	 * @return True: if the application process exists in the configuration.
	 */
	bool checkApplicationInConfiguration(Message& request, ApplicationProcessId applicationID, uint8_t numOfServices);

	/**
	 * Checks whether the requested service type is present in the application process configuration.
	 * Reports a NonExistentServiceTypeDefinition error if it is not, skipping the necessary amount of bytes in the
	 * request.
	 *
	 * @return True: if the service type exists in the configuration.
	 */
	bool checkServiceTypeInConfiguration(Message& request, ApplicationProcessId applicationID,
	    ServiceTypeNum serviceType, uint8_t numOfMessages);

	/**
	 * Checks whether the requested report type is present in the application process configuration.
	 * Reports a NonExistentReportTypeDefinition error if it is not.
	 *
	 * @return True: if the report type exists in the configuration.
	 */
	bool checkReportTypeInConfiguration(const Message& request, ApplicationProcessId applicationID,
	    ServiceTypeNum serviceType, MessageTypeNum messageType) const;

	/**
	 * Deletes every definition containing the requested application process ID, from the application process
	 * configuration.
	 */
	void deleteApplicationProcess(ApplicationProcessId applicationID);

	/**
	 * Deletes the requested service type definition from the application process configuration. The deletion of the
	 * last service type definition of an application process implicitly deletes the application process definition.
	 */
	void deleteServiceType(ApplicationProcessId applicationID, ServiceTypeNum serviceType);

	/**
	 * Deletes the requested report type from the application process configuration. If the deletion results in an
	 * empty service type definition, it deletes the corresponding service type definition as well.
	 */
	void deleteReportType(ApplicationProcessId applicationID, ServiceTypeNum serviceType, MessageTypeNum messageType);

	/**
	 * @return true if the specified application process exists in the definitions map, false otherwise.
	 */
	bool isApplicationEnabled(ApplicationProcessId targetAppID) const;

	/**
	 * @return true if the pair of applicationID and serviceType already exists in the definitions map, false otherwise.
	 */
	bool isServiceAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const;

	/**
	 * @return true if the specified report type exists in the definition of the specified application process and
	 * service type, false otherwise.
	 */
	bool isReportTypeAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
	    MessageTypeNum messageType) const;

private:
	/**
	 * @return true if the maximum number of service type definitions per application process has been reached,
	 * false otherwise.
	 */
	bool isMaxServiceTypesReached(ApplicationProcessId applicationID) const;
};
