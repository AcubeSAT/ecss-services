#ifndef ECSS_SERVICES_ECSS_DEFINITIONS_H
#define ECSS_SERVICES_ECSS_DEFINITIONS_H

#define ECSS_MAX_MESSAGE_SIZE 1024U

#define ECSS_MAX_STRING_SIZE 256U

#define ECSS_MAX_FIXED_OCTET_STRING_SIZE 256U // For the ST13 large packet transfer service

/**
 * The total number of different message types that can be handled by this project
 */
#define ECSS_TOTAL_MESSAGE_TYPES (10 * 20)

// 7.4.1
#define CCSDS_PACKET_VERSION 0

// 7.4.4.1c
#define ECSS_PUS_VERSION 2U

// 9.3.1a.1.e
#define ECSS_SEQUENCE_FLAGS 0x3

/**
 * @brief Maximum number of TC requests that can be contained in a single message request
 * @details This definition accounts for the maximum number of TC packet requests that can be
 * contained in the message of a request. This was defined for the time based command scheduling
 * service and specifically to address the needs of the sub-services containing a TC packet in
 * their message request.
 * @attention This definition is probably dependent on the ECSS_TC_REQUEST_STRING_SIZE
 */
#define ECSS_MAX_REQUEST_COUNT 20 // todo: Needs to be defined

/**
 * @brief Maximum length of a String converted TC packet message
 * @details This definition refers to the maximum length that an embedded TC packet, meaning a TC
 * packet contained in a message request as a part of the request.
 */
#define ECSS_TC_REQUEST_STRING_SIZE 64

// todo: Define the maximum number of activities
#define ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES 10

/**
 * @brief Time margin used in the time based command scheduling service ST[11]
 * @details This defines the time margin in seconds, from the current rime, that an activity must
 * have in order
 * @todo Define the time margin for the command activation
 */
#define ECSS_TIME_MARGIN_FOR_ACTIVATION 60

/**
 * @brief Size of the multimap that holds every event-action definition
 */
#define ECSS_EVENT_ACTION_STRUCT_MAP_SIZE 256

// todo: Define the maximum delta between the specified
#define ECSS_MAX_DELTA_OF_RELEASE_TIME 60
// release time and the actual release time

#endif // ECSS_SERVICES_ECSS_DEFINITIONS_H
