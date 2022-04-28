#ifndef ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP
#define ECSS_SERVICES_REALTIMEFORWARDINGCONTROLSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/ForwardControlConfiguration.hpp"
#include "Service.hpp"
#include "etl/vector.h"

/**
 * Implementation of the ST[14] 'Real Time Forwarding Control Service' as defined in ECSS-E-ST-70-41C.
 *
 * @brief
 * The purpose of this Service is to control the forwarding of the stores' telemetry to the ground station. It includes
 * conditions for all the application processes that are controlled by the Service, which determine whether a message
 * should be forwarded to the ground station, through the corresponding virtual channel.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class RealTimeForwardingControlService {
public:
	inline static const uint8_t ServiceType = 14;

	enum MessageType : uint8_t {
		DeleteReportTypesFromAppProcessConfiguration = 2,
		EventReportConfigurationContentReport = 16,
	};

	RealTimeForwardingControlService() = default;

	/**
	 * Contains the Application IDs, controlled by the Service.
	 */
	etl::vector<uint8_t, ECSSMaxControlledApplications> controlledApplications;

	/**
	 * The Application Process configuration, containing all the application process, service type and message type
	 * definitions.
	 */
	ForwardControlConfiguration::ApplicationProcess applicationProcessConfiguration;

private:
	/**
	 * Checks whether the specified message type already exists in the specified application process and service
	 * type definition.
	 */
	bool reportExistsInAppProcessConfiguration(uint8_t target, uint8_t applicationID, uint8_t serviceType);

	/**
	 * Checks whether the requested application is present in the application process configuration.
	 */
	bool applicationExists(Message& request, uint8_t applicationID, uint8_t numOfServices);

	/**
	 * Checks whether the requested service type is present in the application process configuration.
	 */
	bool serviceTypeExists(Message& request, uint8_t applicationID, uint8_t serviceType, uint8_t numOfMessages);

	/**
	 * Checks whether the requested report type is present in the application process configuration.
	 */
	bool reportTypeExists(Message& request, uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

	/**
	 * Deletes the requested service type from the application process configuration. If the deletion results in an
	 * empty application process, it deletes the corresponding application process definition as well.
	 */
	void deleteServiceRecursive(uint8_t applicationID, uint8_t serviceType);

	/**
	 * Deletes the requested report type from the application process configuration. If the deletion results in an
	 * empty service, it deletes the corresponding service. If the deletion of the service, results in an empty
	 * application process, it deletes the corresponding application process definition as well.
	 */
	void deleteReportRecursive(uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

public:
	/**
	 * TC[14,2] 'Delete report types from the application process forward control configuration'.
	 */
	void deleteReportTypesFromAppProcessConfiguration(Message& request);

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
