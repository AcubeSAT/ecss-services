/**
 * Configuration for the ETL standard library replacement
 *
 * Packaged with the library headers, so it also applies to code that uses them.
 */

#ifndef ECSS_SERVICES_ETL_PROFILE_H
#define ECSS_SERVICES_ETL_PROFILE_H

#define ETL_VERBOSE_ERRORS
#define ETL_CHECK_PUSH_POP
#define ETL_LOG_ERRORS
// Do not define ETL_THROW_EXCEPTIONS. ETL errors are logged, not thrown.

#endif // ECSS_SERVICES_ETL_PROFILE_H
