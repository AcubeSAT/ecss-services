#include <Logger.hpp>

// Reimplementation of the function for variable C strings
template <>
Logger::LogEntry& operator<<(Logger::LogEntry& entry, char* value) {
	entry.message.append(value);
	return entry;
}

// Reimplementation of the function for C strings
template <>
Logger::LogEntry& operator<<(Logger::LogEntry& entry, const char* value) {
	entry.message.append(value);
	return entry;
}

Logger::LogEntry::LogEntry(LogLevel level) : level(level) {
	format.precision(3); // Set precision to 3 decimal digits
}

Logger::LogEntry::~LogEntry() {
	// When the destructor is called, the log message is fully "designed". Now we can finally "display" it to the user.
	Logger::log(level, message);
}
