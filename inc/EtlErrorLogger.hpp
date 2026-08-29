#ifndef ECSS_SERVICES_ETL_ERROR_LOGGER_HPP
#define ECSS_SERVICES_ETL_ERROR_LOGGER_HPP

#include <Logger.hpp>
#include <etl/error_handler.h>
#include <etl/string.h>
#include <etl/to_string.h>

#if ETL_USING_EXCEPTIONS
#error "ETL C++ exceptions must remain disabled; use ETL_LOG_ERRORS only."
#endif

inline void logEtlError(const etl::exception& etlError) {
	etl::string<LOGGER_MAX_MESSAGE_SIZE> message;
	message.append("ETL Error: ");
	message.append(etlError.what());
	message.append(" in file: ");
	message.append(etlError.file_name());
	message.append(" at line: ");
	etl::to_string(etlError.line_number(), message, true);
	Logger::log(Logger::error, message);
}

inline void registerEtlErrorLogger() {
	etl::error_handler::set_callback<logEtlError>();
}

#endif // ECSS_SERVICES_ETL_ERROR_LOGGER_HPP
