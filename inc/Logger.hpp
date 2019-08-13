#ifndef ECSS_SERVICES_LOGGER_HPP
#define ECSS_SERVICES_LOGGER_HPP

#include <cstdint>
#include <etl/String.hpp>
#include <etl/to_string.h>
#include <ECSS_Definitions.hpp>

#if defined LOGLEVEL_TRACE
#define LOGLEVEL Logger::trace // Ignore-MISRA
#elif defined LOGLEVEL_DEBUG
#define LOGLEVEL Logger::debug // Ignore-MISRA
#elif defined LOGLEVEL_INFO
#define LOGLEVEL Logger::info // Ignore-MISRA
#elif defined LOGLEVEL_NOTICE
#define LOGLEVEL Logger::notice // Ignore-MISRA
#elif defined LOGLEVEL_WARNING
#define LOGLEVEL Logger::warning // Ignore-MISRA
#elif defined LOGLEVEL_ERROR
#define LOGLEVEL Logger::error // Ignore-MISRA
#elif defined LOGLEVEL_EMERGENCY
#define LOGLEVEL Logger::emergency // Ignore-MISRA
#else
#define LOGLEVEL Logger::disabled // Ignore-MISRA
#endif

/**
 * Create a stream to log a Message
 *
 * This functions appends one line to the Logs (which could be printed to screen, transferred via UART or stored for
 * later use.)
 *
 * Examples of usage: \n
 * `LOG(Logger::debug) << "Reached point of no return";` \n
 * `LOG(Logger::error) << "More than " << 50 << " dogs found!";`
 *
 * You can also use one of the \ref LOG_TRACE, \ref LOG_DEBUG, \ref LOG_INFO, \ref LOG_NOTICE, \ref LOG_WARNING,
 * \ref LOG_ERROR or \ref LOG_EMERGENCY defines, which avoid the need of explicitly passing the log level: \n
 * `LOG_DEBUG << "Reached point of no return";` \n
 * `LOG_ERROR << "More than " << 50 << " dogs found!";`
 *
 * See \ref Logger::LogLevel for an explanation of the different log levels.
 *
 * @par Implementation details
 * This macro uses a trick to pass an object where the `<<` operator can be used, and which is logged when the statement
 * is complete. It uses an `if` statement, initializing a variable within its condition. According to the C++98
 * standard (1998), Clause 3.3.2.4, "Names declared in the [..] condition of the if statement are local to the if [...]
 * statement (including the controlled statement) [...]". This result in the \ref Logger::LogEntry::~LogEntry()
 * to be called as soon as the statement is complete. The bottom `if` statement serves this purpose, and is always
 * evaluated to true to ensure execution.
 *
 * @par
 * Additionally, the top `if` checks the sufficiency of the log level. It should be optimized away at compile-time on
 * invisible log entries, meaning that there is no performance overhead for unused calls to LOG.
 *
 * @section GlobalLogLevels Global log levels
 * The **global log level** defines the minimum severity of events to be displayed. Log entries with a severity equal
 * to or higher than the global log level will be shown. Log entries with a severity smaller than the global log level
 * will not be shown.
 *
 * The global log level can be set by defining one of the following constants:
 * - `LOGLEVEL_TRACE`
 * - `LOGLEVEL_DEBUG`
 * - `LOGLEVEL_INFO`
 * - `LOGLEVEL_NOTICE`
 * - `LOGLEVEL_WARNING`
 * - `LOGLEVEL_ERROR`
 * - `LOGLEVEL_EMERGENCY`
 *
 * @relates Logger
 * @param level The log level. A value of \ref Logger::LogEntry
 */
#define LOG(level)  /* Ignore-MISRA */ \
    if (Logger::isLogged(level)) \
        if (Logger::LogEntry entry(level); true) \
            entry

#define LOG_TRACE     LOG(Logger::trace)     ///< @see LOG @relates Logger
#define LOG_DEBUG     LOG(Logger::debug)     ///< @see LOG @relates Logger
#define LOG_INFO      LOG(Logger::info)      ///< @see LOG @relates Logger
#define LOG_NOTICE    LOG(Logger::notice)    ///< @see LOG @relates Logger
#define LOG_WARNING   LOG(Logger::warning)   ///< @see LOG @relates Logger
#define LOG_ERROR     LOG(Logger::error)     ///< @see LOG @relates Logger
#define LOG_EMERGENCY LOG(Logger::emergency) ///< @see LOG @relates Logger

/**
 * A logging class for ECSS Services that supports ETL's String and is lightweight enough to be used in embedded
 * development.
 *
 * @note Always use the \ref LOG macro and its associated utility macros to log. Do not directly use the Logger class.
 */
class Logger {
public:
	/**
	 * No need to instantiate a Logger object for now.
	 */
	Logger() = delete;

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
		String<LOGGER_MAX_MESSAGE_SIZE> message = ""; ///< The current log message itself, starting from a blank slate
		etl::format_spec format; ///< ETL's string format specification
		LogLevel level; ///< The log level of this message

		explicit LogEntry(LogLevel level); ///< Create a new LogEntry

		/**
		 * The LogEntry destructor gets called whenever a log message is finalized, and ready to be shown to the
		 * user. This function is responsible for calling the Logger::log function.
		 */
		~LogEntry();

		LogEntry(LogEntry const&) = delete; ///< Unimplemented copy constructor
	};

	/**
	 * Returns whether a log entry of level \p level is logged, based on the compilation constants
	 * @param level The level of the log entry
	 * @return True if the logging is enabled for \p level, false if not
	 */
	static constexpr bool isLogged(LogLevelType level) {
		return static_cast<LogLevelType>(LOGLEVEL) <= level;
	}

	/**
	 * Store a new log message
	 */
	static void log(LogLevel level, String<LOGGER_MAX_MESSAGE_SIZE>& message);
};

/**
 * Stream operator to append new values to a log record
 *
 * @relates Logger
 * @tparam T The type of value to append
 * @param entry The already existing Logger::LogEntry
 * @param value The new value to add
 * @return The new Logger::LogEntry where the value has been appended
 */
template <class T>
Logger::LogEntry& operator<<(Logger::LogEntry& entry, const T value) {
	etl::to_string(value, entry.message, entry.format, true);

	return entry;
}

Logger::LogEntry& operator<<(Logger::LogEntry& entry, const std::string & value);

#endif //ECSS_SERVICES_LOGGER_HPP
