#include <Logger.hpp>

// Reimplementation of the function for variable C strings
template <>
Logger::LogEntry& Logger::LogEntry::operator<<(char* value) {
	message.append(value);
	return *this;
}

// Reimplementation of the function for C strings
template <>
Logger::LogEntry& Logger::LogEntry::operator<<(const char* value) {
	message.append(value);
	return *this;
}

Logger::LogEntry::LogEntry(LogLevel level) : level(level) {
	format.precision(3); // Set precision to 3 decimal digits
}

Logger::LogEntry::~LogEntry() {
	// When the destructor is called, the log message is fully "designed". Now we can finally "display" it to the user.
	Logger::log(level, message);
}
