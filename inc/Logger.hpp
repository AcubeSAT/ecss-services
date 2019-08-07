#ifndef ECSS_SERVICES_LOGGER_HPP
#define ECSS_SERVICES_LOGGER_HPP

#include <cstdint>
#include <etl/String.hpp>
#include <etl/to_string.h>
#include <ECSS_Definitions.hpp>

#if defined LOGLEVEL_TRACE
#define LOGLEVEL Logger::trace
#elif defined LOGLEVEL_DEBUG
#define LOGLEVEL Logger::debug
#elif defined LOGLEVEL_INFO
#define LOGLEVEL Logger::info
#elif defined LOGLEVEL_NOTICE
#define LOGLEVEL Logger::notice
#elif defined LOGLEVEL_WARNING
#define LOGLEVEL Logger::warning
#elif defined LOGLEVEL_ERROR
#define LOGLEVEL Logger::error
#elif defined LOGLEVEL_EMERGENCY
#define LOGLEVEL Logger::emergency
#else
#define LOGLEVEL Logger::disabled
#endif

#define _ac_LOGGER_ENABLED_LEVEL(level) (( (Logger::LogLevelType) LOGLEVEL) <= ( (Logger::LogLevelType) level))

#define LOG(level) \
    if (_ac_LOGGER_ENABLED_LEVEL(level)) \
        if (Logger::LogEntry entry(level); true) \
            entry

#define LOG_TRACE     LOG(Logger::trace)
#define LOG_DEBUG     LOG(Logger::debug)
#define LOG_INFO      LOG(Logger::info)
#define LOG_NOTICE    LOG(Logger::notice)
#define LOG_WARNING   LOG(Logger::warning)
#define LOG_ERROR     LOG(Logger::error)
#define LOG_EMERGENCY LOG(Logger::emergency)

/**
 * A logging class for ECSS Services that supports ETL's String and is lightweight enough to be used in embedded
 * development.
 */
class Logger {
private:

protected:

	/**
	 * Default protected constructor for this Logger
	 */
	Logger() = default;

public:
	/**
	 * The underlying type to be used for values of Logger::LogLevel.
	 */
	typedef uint8_t LogLevelType;

	/**
	 * Log levels supported by the logger. Each level represents a different severity of the logged Message,
	 * and messages of lower severities can be filtered on top of more significant ones.
	 *
	 * Each severity is tied to a number. The higher the number, the higher the severity.
	 */
	enum LogLevel {
		trace = 32, ///< Very detailed information, useful for tracking the individual steps of an operation
		debug = 64, ///< General debugging information
		info = 96, ///< Noteworthy or periodical events
		notice = 128, ///< Uncommon but expected events
		warning = 160, ///< Unexpected events that do not compromise the operability of a function
		error = 192, ///< Unexpected failure of an operation
		emergency = 254, ///< Unexpected failure that renders the entire system unusable
		disabled = 255, ///< Use this log level to disable logging entirely. No message should be logged as disabled.
	};

	/**
	 * A class that defines a log message.
	 *
	 * Instead of using this class, prefer one of the above macros
	 */
	struct LogEntry {
		String<LOGGER_MAX_MESSAGE_SIZE> message = ""; ///< The current log message itself
		etl::format_spec format; ///< ETL's string format specification
		LogLevel level;

		explicit LogEntry(LogLevel level); ///< Create a new LogEntry

		/**
		 * The LogEntry destructor gets called whenever a log message is finalized, and ready to be shown to the
		 * user. This function is responsible for calling the Logger::log function.
		 */
		~LogEntry();

		LogEntry(LogEntry const&) = delete; ///< Unimplemented copy constructor
	};

	/**
	 * @brief Unimplemented copy constructor
	 *
	 * Does not allow Loggers to be copied. There should be only one instance for each Logger.
	 */
	Logger(Logger const&) = delete;

	/**
	 * Unimplemented assignment operation
	 *
	 * Does not allow changing the instances of Loggers, as Loggers are singletons.
	 */
	void operator=(Logger const&) = delete;

	/**
	 * Default destructor
	 */
	~Logger() = default;

	/**
	 * Default move constructor
	 */
	Logger(Logger&& service) noexcept = default;

	/**
	 * Default move assignment operator
	 */
	Logger& operator=(Logger&& service) noexcept = default;

	/**
	 * Store a new log message
	 */
	static void log(LogLevel level, String<LOGGER_MAX_MESSAGE_SIZE>& message);
};

template <class T>
Logger::LogEntry& operator<<(Logger::LogEntry& entry, const T value) {
	etl::to_string(value, entry.message, entry.format, true);

	return entry;
}


#endif //ECSS_SERVICES_LOGGER_HPP
