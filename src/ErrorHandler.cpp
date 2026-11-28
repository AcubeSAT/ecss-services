#include "ECSS_Configuration.hpp"
#include <ErrorHandler.hpp>
#include <Logger.hpp>
#include <ServicePool.hpp>
#include <etl/error_handler.h>
#include "Services/RequestVerificationService.hpp"

template <>
void ErrorHandler::reportError(const Message& message, AcceptanceErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failAcceptanceVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, ExecutionStartErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failStartExecutionVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

void ErrorHandler::reportProgressError(const Message& message, ExecutionProgressErrorType errorCode, StepId stepID) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failProgressExecutionVerification(message, errorCode, stepID);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, ExecutionCompletionErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failCompletionExecutionVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, RoutingErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failRoutingVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

void ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType errorCode, const char* file, int line) {
	logError(errorCode);

	if (file == nullptr) {
		return;
	}

	const char* suffix = file;
	for (const char* cursor = suffix; *cursor != '\0'; ++cursor) {
		if ((*cursor == '/') || (*cursor == '\\')) {
			suffix = cursor + 1;
		}
	}

	if (suffix[0] != '\0') {
		LOG_ERROR << suffix << ":" << line;
	}
}

namespace {
	void logEtlError(const etl::exception& etlError) {
		etl::string<LOGGER_MAX_MESSAGE_SIZE> message;
		message.append("ETL Error: ");
		message.append(etlError.what());
		message.append(" in file: ");
		message.append(etlError.file_name());
		message.append(" at line: ");
		etl::to_string(etlError.line_number(), message, true);
		Logger::log(Logger::error, message);
	}
} // namespace

void ErrorHandler::registerEtlErrorLogger() {
	etl::error_handler::set_callback<logEtlError>();
}
