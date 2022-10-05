/**
 * This file specifies the logging utilities for x86 desktop platforms. These logging functions
 * just print the error to screen (via stderr).
 */

#include <ECSSMessage.hpp>
#include <ErrorHandler.hpp>
#include <Logger.hpp>
#include <cxxabi.h>
#include <iostream>
#include <type_traits>

// TODO: Find a way to reduce the number of copies of this chunk
template void ErrorHandler::logError(const ECSSMessage&, ErrorHandler::AcceptanceErrorType);
template void ErrorHandler::logError(const ECSSMessage&, ErrorHandler::ExecutionStartErrorType);
template void ErrorHandler::logError(const ECSSMessage&, ErrorHandler::ExecutionProgressErrorType);
template void ErrorHandler::logError(const ECSSMessage&, ErrorHandler::ExecutionCompletionErrorType);
template void ErrorHandler::logError(const ECSSMessage&, ErrorHandler::RoutingErrorType);
template void ErrorHandler::logError(ErrorHandler::InternalErrorType);

template <typename ErrorType>
void ErrorHandler::logError(const ECSSMessage& message, ErrorType errorType) {
	LOG_ERROR
	    /*
	     * Gets the error class name from the template
	     * Note: This is g++-dependent code and should only be used for debugging.
	     */
	    << abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr) << " Error "
	    << "[" << static_cast<uint16_t>(message.serviceType) << "," << static_cast<uint16_t>(message.messageType)
	    << "]: " << std::underlying_type_t<ErrorType>(errorType);
}

template <typename ErrorType>
void ErrorHandler::logError(ErrorType errorType) {
	LOG_ERROR
	    /*
	     * Gets the error class name from the template
	     * Note: This is g++-dependent code and should only be used for debugging.
	     */
	    << abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr)
	    << " Error: "
	    << std::underlying_type_t<ErrorType>(errorType);
}
