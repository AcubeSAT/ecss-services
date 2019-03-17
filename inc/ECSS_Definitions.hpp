#ifndef ECSS_SERVICES_ECSS_DEFINITIONS_H
#define ECSS_SERVICES_ECSS_DEFINITIONS_H

// Todo: Specify maximum size for regular messages
#define ECSS_MAX_MESSAGE_SIZE 1024

#define ECSS_MAX_STRING_SIZE 256

// 7.4.1
#define CCSDS_PACKET_VERSION 0

// 7.4.4.1c
#define ECSS_PUS_VERSION 2

// 9.3.1a.1.e
#define ECSS_SEQUENCE_FLAGS 0x3

#define ECSS_MAX_REQUEST_COUNT 20 // todo: Needs to be defined

#define ECSS_TC_REQUEST_STRING_SIZE 64

// todo: Define the maximum number of activities
#define ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES    10

// todo: Define the time margin for the command activation
#define ECSS_TIME_MARGIN_FOR_ACTIVATION  60

// todo: Define the maximum delta between the specified
#define ECSS_MAX_DELTA_OF_RELEASE_TIME   60
// release time and the actual release time

#endif //ECSS_SERVICES_ECSS_DEFINITIONS_H
