/**
 * Configuration for the ETL standard library replacement
 */

#ifndef ECSS_SERVICES_ETL_PROFILE_H
#define ECSS_SERVICES_ETL_PROFILE_H

#define ETL_VERBOSE_ERRORS
#define ETL_CHECK_PUSH_POP

// Only GCC is used as a compiler
#include "etl/profiles/gcc_linux_x86.h"

#endif // ECSS_SERVICES_ETL_PROFILE_H
