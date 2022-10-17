/**
 * This file specifies the logging utilities for x86 desktop platforms. These logging functions
 * just print the error to screen (via stderr).
 */

#include <iostream>
#include <cxxabi.h>
#include <ErrorHandler.hpp>
#include <Message.hpp>
#include <Logger.hpp>
#include <type_traits>
#include "Helpers/EnumMagic.hpp"

// TODO: Find a way to reduce the number of copies of this chunk
template void ErrorHandler::logError(const Message&, ErrorHandler::AcceptanceErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionStartErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionProgressErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionCompletionErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::RoutingErrorType);
template void ErrorHandler::logError(ErrorHandler::InternalErrorType);

template <typename ErrorType>
void ErrorHandler::logError(const Message& message, ErrorType errorType) {
	LOG_ERROR
	    /*
	     * Gets the error class name from the template
	     * Note: This is g++-dependent code and should only be used for debugging.
	     */
	    << abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr) << " Error "
	    << "[" << static_cast<uint16_t>(message.serviceType) << "," << static_cast<uint16_t>(message.messageType)
	    << "]: " << enumName(errorType) << " (" << static_cast<std::underlying_type_t<ErrorType>>(errorType) << ")";
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
	    << enumName(errorType) << " (" << static_cast<std::underlying_type_t<ErrorType>>(errorType) << ")";
}
