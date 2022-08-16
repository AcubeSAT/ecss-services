#ifndef ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP
#define ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/AllMessageTypes.hpp"
#include "Helpers/ForwardControlConfiguration.hpp"
#include "Service.hpp"
#include "etl/vector.h"

/**
 * Implementation of the ST[14] 'Real Time Forwarding Control Service' as defined in ECSS-E-ST-70-41C.
 *
 * The purpose of this Service is to control the forwarding of the telemetry to the ground station. It includes
 * conditions for all the application processes that are controlled by the Service, which determine whether a message
 * should be forwarded to the ground station, through the corresponding virtual channel.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class RealTimeForwardingControlService : Service {
public:
	inline static const uint8_t ServiceType = 14;

	enum MessageType : uint8_t {
		AddReportTypesToAppProcessConfiguration = 1,
	};

	RealTimeForwardingControlService() {
		serviceType = ServiceType;
	}

	/**
	 * Contains the Application IDs, controlled by the Service.
	 */
	etl::vector<uint8_t, ECSSMaxControlledApplicationProcesses> controlledApplications;

	/**
	 * The Application Process configuration, containing all the application process, service type and message type
	 * definitions.
	 */
	ApplicationProcessConfiguration applicationProcessConfiguration;

private:
	/**
	 * Adds all report types of the specified application process definition, to the application process configuration.
	 */
	void addAllReportsOfApplication(uint8_t applicationID);

	/**
	 * Adds all report types of the specified service type, to the application process configuration.
	 */
	void addAllReportsOfService(uint8_t applicationID, uint8_t serviceType);

	/**
	 * Counts the number of service types, stored for the specified application process.
	 */
	uint8_t countServicesOfApplication(uint8_t applicationID);

	/**
	 * Counts the number of report types, stored for the specified service type.
	 */
	uint8_t countReportsOfService(uint8_t applicationID, uint8_t serviceType);

	/**
	 * Checks whether the specified message type already exists in the specified application process and service
	 * type definition.
	 */
	bool reportExistsInAppProcessConfiguration(uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

	/**
	 * Performs the necessary error checking/logging for a specific application process ID. Also, skips the necessary
	 * bytes from the request message, in case of an invalid request.
	 *
	 * @return True: if the application is valid and passes all the necessary error checking.
	 */
	bool checkApplicationOfAppProcessConfig(Message& request, uint8_t applicationID, uint8_t numOfServices);

	/**
	 * Checks if the specified application process is controlled by the Service and returns true if it does.
	 */
	bool checkAppControlled(Message& request, uint8_t applicationId);

	/**
	 * Checks if all service types are allowed already, i.e. if the application process contains no service type
	 * definitions.
	 */
	bool allServiceTypesAllowed(Message& request, uint8_t applicationID);

	/**
	 * Checks if the maximum number of service type definitions per application process is reached.
	 */
	bool maxServiceTypesReached(Message& request, uint8_t applicationID);

	/**
	 * Performs the necessary error checking/logging for a specific service type. Also, skips the necessary bytes
	 * from the request message, in case of an invalid request.
	 *
	 * @return True: if the service type is valid and passes all the necessary error checking.
	 */
	bool checkService(Message& request, uint8_t applicationID, uint8_t numOfMessages);

	/**
	 * Checks if the maximum number of report type definitions per service type definition is reached.
	 */
	bool maxReportTypesReached(Message& request, uint8_t applicationID, uint8_t serviceType);

	/**
	 * Checks if the maximum number of message types that can be contained inside a service type definition, is
	 * already reached.
	 *
	 * @return True: if the message type is valid and passes all the necessary error checking.
	 */
	bool checkMessage(Message& request, uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

public:
	/**
	 * TC[14,1] 'Add report types to the application process forward control configuration'.
	 */
	void addReportTypesToAppProcessConfiguration(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a TC packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice.
	 */
	void execute(Message& message);
};

#endif
