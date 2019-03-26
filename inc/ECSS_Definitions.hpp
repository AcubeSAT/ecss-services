#ifndef ECSS_SERVICES_ECSS_DEFINITIONS_H
#define ECSS_SERVICES_ECSS_DEFINITIONS_H

#define ECSS_MAX_MESSAGE_SIZE 1024

#define ECSS_MAX_STRING_SIZE 256

// 7.4.1
#define CCSDS_PACKET_VERSION 0

/**
 * @brief Maximum length of a String converted TC packet message
 * @details This definition refers to the maximum length that an embedded TC packet, meaning a TC
 * packet contained in a message request as a part of the request.
 */
#define ECSS_TC_REQUEST_STRING_SIZE 64

// 7.4.4.1c
#define ECSS_PUS_VERSION 2

// 9.3.1a.1.e
#define ECSS_SEQUENCE_FLAGS 0x3

#endif //ECSS_SERVICES_ECSS_DEFINITIONS_H
