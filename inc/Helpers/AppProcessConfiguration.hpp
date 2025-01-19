#pragma once

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/Parameter.hpp"
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
	void addAllReportsOfApplication(ApplicationProcessId applicationID) {
		for (const auto& service: AllReportTypes::MessagesOfService) {
			uint8_t const serviceType = service.first;
			addAllReportsOfService(applicationID, serviceType);
		}
	}

	/**
	 * Adds all report types of the specified service type, to the application process configuration.
	 */
	void addAllReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
		for (const auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
			auto appServicePair = std::make_pair(applicationID, serviceType);
			definitions[appServicePair].push_back(messageType);
		}
	}

	/**
	 * Counts the number of service types, stored for the specified packet store ID and application process.
	 */
	uint8_t countServicesOfApplication(ApplicationProcessId applicationID) {
		return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](const auto& definition) { return applicationID == definition.first.first; });
	}

	/**
	 * Counts the number of report types, stored for the specified service type.
	 */
	uint8_t countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		return definitions[appServicePair].size();
	}

	/**
	 * Checks whether the specified message type already exists in the specified packet store ID, application process and service
	 * type definition.
	 */
	bool reportExistsInAppProcessConfiguration(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
																			 MessageTypeNum messageType) {
		auto key = std::make_pair(applicationID, serviceType);
		auto& messages = definitions[key];
		return std::find(messages.begin(), messages.end(), messageType) != messages.end();
	}

	/**
	 * Performs the necessary error checking/logging for a specific packet store ID and application process ID. Also, skips the necessary
	 * bytes from the request message, in case of an invalid request.
	 *
	 * @return True: if the application is valid and passes all the necessary error checking.
	 */
	bool checkApplicationOfAppProcessConfig(Message& request, ApplicationProcessId applicationID,
																			  uint8_t numOfServices,
																			  etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications,) {
		if (not checkAppControlled(controlledApplications, request, applicationID) or allServiceTypesAllowed(request,
		applicationID)) {
			for (uint8_t i = 0; i < numOfServices; i++) {
				request.skipBytes(sizeof(ServiceTypeNum));
				uint8_t const numOfMessages = request.readUint8();
				request.skipBytes(numOfMessages);
			}
			return false;
		}
		return true;
	}

	/**
	 * Checks if all service types are allowed already, i.e. if the application process contains no service type
	 * definitions.
	 */
	bool allServiceTypesAllowed(const Message& request, ApplicationProcessId applicationID) {
		if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
			return true;
		}
		return false;
	}

	/**
	 * Checks if the specified application process is controlled by the Service and returns true if it does.
	 */
	bool checkAppControlled(etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications, const
	Message& request,
	ApplicationProcessId applicationID) {
		if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationID) ==
			controlledApplications.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
			return false;
			}
		return true;
	}

	/**
	 * Checks if the maximum number of service type definitions per application process is reached.
	 */
	bool checkMaxServiceTypesReached(const Message& request, ApplicationProcessId applicationID) {
		if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
			return true;
		}
		return false;
	}

	/**
	 * Performs the necessary error checking/logging for a specific service type. Also, skips the necessary bytes
	 * from the request message, in case of an invalid request.
	 *
	 * @return True: if the service type is valid and passes all the necessary error checking.
	 */
	bool checkService(Message& request, ApplicationProcessId applicationID, uint8_t numOfMessages) {
		if (checkMaxServiceTypesReached(request, applicationID)) {
			request.skipBytes(numOfMessages);
			return false;
		}
		return true;
	}

	/**
	 * Checks if the maximum number of report type definitions per service type definition is reached.
	 */
	bool checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum
	serviceType)  {
		if (countReportsOfService(applicationID, serviceType) >= AllReportTypes::MessagesOfService.at(serviceType).size
		()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
			return true;
		}
		return false;
	}

	/**
	 * Checks if the maximum number of message types that can be contained inside a service type definition, is
	 * already reached.
	 *
	 * @return True: if the message type is valid and passes all the necessary error checking.
	 */
	bool checkMessage(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum serviceType, MessageTypeNum messageType)  {
		return !checkMaxReportTypesReached(request, applicationID, serviceType) or
			   !reportExistsInAppProcessConfiguration(applicationID, serviceType, messageType);
	}

	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ReportTypeDefinitions;

	/**
	 * This is the key for the application process configuration map. It contains a pair with the applicationID and
	 * the serviceType.
	 */
	typedef std::pair<uint8_t, uint8_t> AppServiceKey;

	/**
	 * Map containing the report type definitions. Each application process has its own ID. The combination of the
	 * application ID and the service type is used as a key to provide access to the list of report type definitions.
	 *
	 * @note
	 * The report type definitions are basically the message types of each service. For example a message type for the
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