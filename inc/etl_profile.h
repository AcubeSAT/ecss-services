/**
 * Configuration for the ETL standard library replacement
 *
 * Firmware compiles ETL with this profile too.
 */

#ifndef ECSS_SERVICES_ETL_PROFILE_H
#define ECSS_SERVICES_ETL_PROFILE_H

#define ETL_VERBOSE_ERRORS
#define ETL_CHECK_PUSH_POP
#define ETL_LOG_ERRORS

#ifdef ETL_THROW_EXCEPTIONS
#error "ETL C++ exceptions must remain disabled; use ETL_LOG_ERRORS only."
#endif

#endif // ECSS_SERVICES_ETL_PROFILE_H
