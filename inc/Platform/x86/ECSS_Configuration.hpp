#ifndef ECSS_SERVICES_ECSS_CONFIGURATION_HPP
#define ECSS_SERVICES_ECSS_CONFIGURATION_HPP

/**
 * @file
 * The configuration file for this ECSS Services platform.
 *
 * @see GlobalLogLevels Define the minimum level for logged messages
 * @see ServiceDefinitions Define the service types that will be compiled
 */

/**
 * @name ServiceDefinitions Service compilation switches
 * These preprocessor defines control whether the compilation of each ECSS service is enabled. By not defining one of
 * those, the service will not be compiled, and no RAM or ROM will be spent storing it.
 *
 * Define these in the `ECSS_Configuration.hpp` file of your platform.
 * @{
 */

#define SERVICE_ALL                       ///< Enables compilation of all the ECSS services
#define SERVICE_DUMMY                     ///<  Compile ST[128] dummy service
#define SERVICE_ENVIRONMENTALTESTINGSERVICE ///<  Compile ST[129] environmental campaign testing service
#define SERVICE_EVENTACTION               ///<  Compile ST[19] event-action
#define SERVICE_EVENTREPORT               ///<  Compile ST[05] event reporting
#define SERVICE_FUNCTION                  ///<  Compile ST[08] function management
#define SERVICE_HOUSEKEEPING              ///<  Compile ST[03] housekeeping
#define SERVICE_LARGEPACKET               ///<  Compile ST[13] large packet transfer
#define SERVICE_MEMORY                    ///<  Compile ST[06] memory management
#define SERVICE_ONBOARDMONITORING         ///<  Compile ST[12] on-board monitoring
#define SERVICE_PARAMETER                 ///<  Compile ST[20] parameter management
#define SERVICE_PARAMETERSTATISTICS       ///<  Compile ST[04] parameter statistics
#define SERVICE_REALTIMEFORWARDINGCONTROL ///<  Compile ST[14] real time forwarding control
#define SERVICE_REQUESTVERIFICATION       ///<  Compile ST[01] request verification
#define SERVICE_STORAGEANDRETRIEVAL       ///<  Compile ST[15] storage-and-retrieval of tm packets
#define SERVICE_TEST                      ///<  Compile ST[17] test
#define SERVICE_TIME                      ///<  Compile ST[09] time management
#define SERVICE_TIMESCHEDULING            ///<  Compile ST[11] time-based scheduling
/** @} */

#endif // ECSS_SERVICES_ECSS_CONFIGURATION_HPP
