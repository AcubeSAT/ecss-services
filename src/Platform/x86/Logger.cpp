#include <Logger.hpp>
#include <etl/String.hpp>
#include <iostream>
#include <Logger_Definitions.hpp>

#include <chrono>
#include <iomanip>

// The implementation of this function appends ANSI codes that should add colours to a compatible terminal
void Logger::log(Logger::LogLevel level, etl::istring & message) {
	// Get the current time & date
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);

	// Get the log level and its colour
	std::string name;
	std::string colour;
	bool keepColour = false; // Whether to keep the colour in the rest of the message

	if (level <= Logger::trace) {
		name = "trace";
		colour = "90"; // bright black
		keepColour = true;
	} else if (level <= Logger::debug) {
		name = "debug";
		colour = "90"; // bright black
	} else if (level <= Logger::info) {
		name = "info";
		colour = "32"; // green
	} else if (level <= Logger::notice) {
		name = "notice";
		colour = "36"; // cyan
	} else if (level <= Logger::warning) {
		name = "warning";
		colour = "33"; // yellow
	} else if (level <= Logger::error) {
		name = "error";
		colour = "31"; // red
	} else {
		name = "emergency";
		colour = "31"; // red
		keepColour = true;
	}

	std::ostringstream ss; // A string stream to create the log message
	ss << "\033" "[0;90m" << std::put_time(&tm, "%FT%T%z") << "\033" "[0m "; // The date
	ss << "[\033" "[1;" << colour << "m" << std::setfill(' ') << std::setw(7) << std::right // Ignore-MISRA
		<< name << std::setw(0) << "\033" "[0m] "; // The log level // Ignore-MISRA

	if (keepColour) {
		ss << "\033" "[0;" << colour << "m"; // Ignore-MISRA
	}
	ss << message.c_str(); // The message itself
	if (keepColour) {
		ss << "\033" "[0m";
	}

	ss << "\n";
	std::cerr << ss.str();
}

// Reimplementation of the log function for C++ strings
// This is kept in the Platform files, since we don't want to mess with std::strings in the microcontroller
Logger::LogEntry& Logger::LogEntry::operator<<(const std::string & value) {
	message.append(value.c_str());

	return *this;
}
