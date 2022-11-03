#ifndef ECSS_SERVICES_ECSS_DEFINITIONS_H
#define ECSS_SERVICES_ECSS_DEFINITIONS_H

#include <chrono>
#include <cstdint>
/**
 * @defgroup ECSSDefinitions ECSS Defined Constants
 *
 * This file contains constant definitions that are used throughout the ECSS services. They often refer to maximum
 * values and upper limits for the storage of data in the services.
 *
 * @todo All these constants need to be redefined and revised after the design and the requirements are finalized.
 *
 * @{
 */

/**
 * @file
 * This file contains constant definitions that are used throughout the ECSS services.
 * @see ECSSDefinitions
 */

/**
 * The maximum size of a regular ECSS message, in bytes
 */
inline const uint16_t ECSSMaxMessageSize = 1024U;

/**
 * The size of each CCSDS Space packet primary header
 */
inline const uint16_t CCSDSPrimaryHeaderSize = 6U;

/**
 * The size of each ECSS Telemetry packet's secondary header
 */
inline const uint16_t ECSSSecondaryTMHeaderSize = 11U;

/**
 * The size of each ECSS Telecommand packet's secondary header
 */
inline const uint16_t ECSSSecondaryTCHeaderSize = 5U;

/**
 * The maximum size of a regular ECSS message, plus its headers and trailing data, in bytes
 */
inline const uint16_t CCSDSMaxMessageSize = ECSSMaxMessageSize + CCSDSPrimaryHeaderSize + ECSSSecondaryTMHeaderSize + 2U;

/**
 * The maximum size of a string to be read or appended to a Message, in bytes
 *
 * This is used by the Message::appendString() and Message::readString() functions
 */
inline const uint16_t ECSSMaxStringSize = 256U;

/**
 * The maximum size of a string to be used by ST[13] \ref LargePacketTransferService, in bytes
 *
 * This is used by the Message::appendString() and Message::readString() functions
 */
inline const uint16_t ECSSMaxFixedOctetStringSize = 256U;

/**
 * The total number of different message types that can be handled by this project
 */
inline const uint8_t ECSSTotalMessageTypes = 10U * 20U;

/**
 * The CCSDS packet version, as specified in section 7.4.1
 */
inline const uint8_t CCSDSPacketVersion = 0;

/**
 * The ECSS packet version, as specified in requirement 7.4.4.1c
 */
inline const uint8_t ECSSPUSVersion = 2U;

/**
 * The CCSDS sequence flags have the constant value 0x3, as specified in section 7.4.1
 */
inline const uint8_t ECSSSequenceFlags = 0x3;

/**
 * @brief Maximum number of TC requests that can be contained in a single message request
 * @details This definition accounts for the maximum number of TC packet requests that can be
 * contained in the message of a request. This was defined for the time based command scheduling
 * service and specifically to address the needs of the sub-services containing a TC packet in
 * their message request.
 * @attention This definition is probably dependent on the ECSS_TC_REQUEST_STRING_SIZE
 */
inline const uint8_t ECSSMaxRequestCount = 20;

/**
 * @brief Maximum length of a String converted TC packet message
 * @details This definition refers to the maximum length that an embedded TC packet, meaning a TC
 * packet contained in a message request as a part of the request.
 */
inline const uint8_t ECSSTCRequestStringSize = 64;

/**
 * The maximum number of activities that can be in the time-based schedule
 * @see TimeBasedSchedulingService
 */
inline const uint8_t ECSSMaxNumberOfTimeSchedActivities = 10;

/**
 * @brief Time margin used in the time based command scheduling service ST[11]
 * @details This defines the time margin in seconds, from the current rime, that an activity must
 * have in order
 * @see TimeBasedSchedulingService
 */
inline constexpr std::chrono::duration<uint8_t> ECSSTimeMarginForActivation(60);

/**
 * @brief Maximum size of an event's auxiliary data
 * @see EventReportService
 */
inline const uint8_t ECSSEventDataAuxiliaryMaxSize = 64;

/**
 * @brief Size of the multimap that holds every event-action definition
 * @see EventActionService
 */
inline const uint16_t ECSSEventActionStructMapSize = 256;

/**
 * The maximum delta between the specified release time and the actual release time
 * @see TimeBasedSchedulingService
 */
inline const uint8_t ECSSMaxDeltaOfReleaseTime = 60;

/**
 * The max number of simply commutated parameters per housekeeping structure in ST[03]
 */
inline const uint16_t ECSSMaxSimplyCommutatedParameters = 30;

/**
 * The number of functions supported by the \ref FunctionManagementService
 */
inline const uint8_t ECSSFunctionMapSize = 5;

/**
 * The maximum length of a function name, in bytes
 * @see FunctionManagementService
 */
inline const uint8_t ECSSFunctionNameLength = 32;

/**
 * The maximum length of the argument of a function
 * @see FunctionManagementService
 */
inline const uint8_t ECSSFunctionMaxArgLength = 32;

/**
 * @brief The maximum size of a log message
 */
inline const uint16_t LoggerMaxMessageSize = 512;

/**
 * @brief Size of the map holding references to each Parameter object for the ST[20] parameter service
 */
inline const uint16_t ECSSParameterCount = 500;

/**
 * @brief Defines whether the optional CRC field is included
 */
inline const bool ECSSCRCIncluded = true;

/**
 * Number of parameters whose statistics we need and are going to be stored into the statisticsMap
 */
inline const uint8_t ECSSMaxStatisticParameters = 4;

/**
 * Whether the ST[04] statistics calculation supports the reporting of stddev
 */
inline const bool SupportsStandardDeviation = true;

/**
 * @brief the max number of bytes allowed for a packet store to handle in ST[15].
 */
inline const uint16_t ECSSMaxPacketStoreSizeInBytes = 1000;

/**
 * @brief the max number of TM packets that a packet store in ST[15] can store
 */
inline const uint16_t ECSSMaxPacketStoreSize = 20;

/**
 * @brief the max number of packet stores that a packet selection subservice can handle in ST[15]
 */
inline const uint16_t ECSSMaxPacketStores = 4;

/**
 * @brief each packet store's id is an etl::string. So this defines the max size of a packet store ID in ST[15]
 */
inline const uint16_t ECSSPacketStoreIdSize = 15;
/**
 * @brief Defines the max number of housekeeping structs that the housekeeping service can contain
 */
inline const uint8_t ECSSMaxHousekeepingStructures = 10;

/**
 * The max number of controlled application processes
 * @see RealTimeForwardingControlService
 */
inline const uint8_t ECSSMaxControlledApplicationProcesses = 5;

/**
 * The max number of report type blocking definitions per service type definition in the application process
 * configuration
 * @see RealTimeForwardingControlService
 * todo: must change when a service with more report types is implemented.
 */
inline const uint8_t ECSSMaxReportTypeDefinitions = 20;

/**
 * The max number of service type definitions per application process type definition in the application process
 * configuration
 * @see RealTimeForwardingControlService
 * todo: must change when all 15 services are implemented.
 */
inline const uint8_t ECSSMaxServiceTypeDefinitions = 10;

/**
 * The number of possible combinations between application processes and service types, i.e. the number of all
 * possible (applicationID, serviceType) pairs.
 */
inline const uint8_t ECSSMaxApplicationsServicesCombinations = ECSSMaxControlledApplicationProcesses *
                                                               ECSSMaxServiceTypeDefinitions;

/**
 * The max number of event definition IDs per event report blocking type definition in the event report blocking
 * configuration
 * @see RealTimeForwardingControlService
 */
inline const uint8_t ECSSMaxEventDefinitionIDs = 15;

/**
 * Limits noting the minimum and maximum valid Virtual Channels used by the Storage and Retrieval subservice
 */
inline const struct {
	uint8_t min = 1;
	uint8_t max = 10;
} VirtualChannelLimits;

/**
 * Maximum number of ST[12] Parameter Monitoring Definitions.
 */
inline const uint8_t ECSSMaxMonitoringDefinitions = 4;

/**
 * 6.18.2.2 The applicationId that is assigned on the specific device that runs these Services.
 * In the ECSS-E-ST-70-41C the application ID is also referred as application process.
 */
inline const uint16_t ApplicationId = 1;

/** @} */
#endif // ECSS_SERVICES_ECSS_DEFINITIONS_H
