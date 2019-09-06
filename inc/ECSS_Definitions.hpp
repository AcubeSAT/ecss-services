#ifndef ECSS_SERVICES_ECSS_DEFINITIONS_H
#define ECSS_SERVICES_ECSS_DEFINITIONS_H

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
#define ECSS_MAX_MESSAGE_SIZE 1024U


/**
 * The maximum size of a regular ECSS message, plus its headers and trailing data, in bytes
 */
#define CCSDS_MAX_MESSAGE_SIZE (ECSS_MAX_MESSAGE_SIZE + 6u + 6u + 2u)

/**
 * The maximum size of a string to be read or appended to a Message, in bytes
 *
 * This is used by the Message::appendString() and Message::readString() functions
 */
#define ECSS_MAX_STRING_SIZE 256U

/**
 * The maximum size of a string to be used by ST[13] \ref LargePacketTransferService, in bytes
 *
 * This is used by the Message::appendString() and Message::readString() functions
 */
#define ECSS_MAX_FIXED_OCTET_STRING_SIZE 256U

/**
 * The total number of different message types that can be handled by this project
 */
#define ECSS_TOTAL_MESSAGE_TYPES (10U * 20U)

/**
 * The CCSDS packet version, as specified in section 7.4.1
 */
#define CCSDS_PACKET_VERSION 0

/**
 * The ECSS packet version, as specified in requirement 7.4.4.1c
 */
#define ECSS_PUS_VERSION 2U

/**
 * The CCSDS sequence flags have the constant value 0x3, as specified in section 7.4.1
 */
#define ECSS_SEQUENCE_FLAGS 0x3

/**
 * @brief Maximum number of TC requests that can be contained in a single message request
 * @details This definition accounts for the maximum number of TC packet requests that can be
 * contained in the message of a request. This was defined for the time based command scheduling
 * service and specifically to address the needs of the sub-services containing a TC packet in
 * their message request.
 * @attention This definition is probably dependent on the ECSS_TC_REQUEST_STRING_SIZE
 */
#define ECSS_MAX_REQUEST_COUNT 20

/**
 * @brief Maximum length of a String converted TC packet message
 * @details This definition refers to the maximum length that an embedded TC packet, meaning a TC
 * packet contained in a message request as a part of the request.
 */
#define ECSS_TC_REQUEST_STRING_SIZE 64

/**
 * The maximum number of activities that can be in the time-based schedule
 * @see TimeBasedSchedulingService
 */
#define ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES 10

/**
 * @brief Time margin used in the time based command scheduling service ST[11]
 * @details This defines the time margin in seconds, from the current rime, that an activity must
 * have in order
 * @see TimeBasedSchedulingService
 */
#define ECSS_TIME_MARGIN_FOR_ACTIVATION 60

/**
 * @brief Maximum size of an event's auxiliary data
 * @see EventReportService
 */
#define ECSS_EVENT_DATA_AUXILIARY_MAX_SIZE 64

/**
 * @brief Size of the multimap that holds every event-action definition
 * @see EventActionService
 */
#define ECSS_EVENT_ACTION_STRUCT_MAP_SIZE 256

/**
 * The maximum delta between the specified release time and the actual release time
 * @see TimeBasedSchedulingService
 */
#define ECSS_MAX_DELTA_OF_RELEASE_TIME 60

/**
 * The maximum number of stored parameters in the \ref ParameterService
 */
#define ECSS_MAX_PARAMETERS 5

/**
 * The number of functions supported by the \ref FunctionManagementService
 */
#define ECSS_FUNCTION_MAP_SIZE 40

/**
 * The maximum length of a function name, in bytes
 * @see FunctionManagementService
 */
#define ECSS_FUNCTION_NAME_LENGTH 20

/**
 * The maximum length of the argument of a function
 * @see FunctionManagementService
 */
#define ECSS_FUNCTION_MAX_ARG_LENGTH 1

/** @} */

/**
 * @brief The maximum size of a log message
 */
#define LOGGER_MAX_MESSAGE_SIZE 512

/**
 * @brief Size of the map holding the Housekeeping structures for the ST[03] housekeeping service
 */
#define ECSS_ST_03_MAX_HOUSEKEEPING_STRUCTURES 20

/**
 * @brief Size of the map holding the Parameter objects for the ST[20] parameter service
 */
#define ECSS_ST_20_MAX_PARAMETERS 40
#endif // ECSS_SERVICES_ECSS_DEFINITIONS_H
