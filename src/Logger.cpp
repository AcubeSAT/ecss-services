#include <Logger.hpp>

template <>
Logger::LogEntry& operator<<(Logger::LogEntry& entry, const char* value) {
	entry.message.append(value);

	return entry;
}

Logger::LogEntry::LogEntry(LogLevel level) : level(level) {
	format.precision(3);
}

Logger::LogEntry::~LogEntry() {
	Logger::log(level, message);
}