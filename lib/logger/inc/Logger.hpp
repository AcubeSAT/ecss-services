#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <etl/String.hpp>
#include <etl/to_string.h>
#include "Logger_Definitions.hpp"

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
#define LOGLEVEL Logger::errECSS_SERVICES_ // Ignore-MISRA
#elif defined LOGLEVEL_EMERGENCY
#define LOGLEVEL Logger::emergency // Ignore-MISRA
#elif !defined LOGLEVEL
#define LOGLEVEL Logger::disabled // Ignore-MISRA
#endif

#define LOG_TRACE     (LOG<Logger::trace>())     ///< @see LOG @relates Logger
#define LOG_DEBUG     (LOG<Logger::debug>())     ///< @see LOG @relates Logger
#define LOG_INFO      (LOG<Logger::info>())      ///< @see LOG @relates Logger
#define LOG_NOTICE    (LOG<Logger::notice>())    ///< @see LOG @relates Logger
#define LOG_WARNING   (LOG<Logger::warning>())   ///< @see LOG @relates Logger
#define LOG_ERROR     (LOG<Logger::error>())     ///< @see LOG @relates Logger
#define LOG_EMERGENCY (LOG<Logger::emergency>()) ///< @see LOG @relates Logger

/**
 * A logging class for ECSS Services that supports ETL's String and is lightweight enough to be used in embedded
 * development.
 *
 * @note Always use the \ref LOG function and its associated utility macros to log. Do not directly use the Logger
 * class.
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
	 * ETL's string format specification, to be used for all logged messages
	 */
	static etl::format_spec format;

	/**
	 * Log levels supported by the logger. Each level represents a different severity of the logged Message,
	 * and messages of lower severities can be filtered on top of more significant ones.
	 *
	 * Each severity is tied to a number. The higher the number, the higher the severity.
	 */
	enum LogLevel : LogLevelType {
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
	 * An empty enum representing a dummy log entry that will not be logged due to an insufficient level.
	 *
	 * @internal
	 */
	enum class NoLogEntry {};

	/**
	 * A class that defines a log message.
	 *
	 * Instead of using this class, prefer one of the above macros.
	 * @see LOG
	 * @internal
	 */
	struct LogEntry {
		String<LOGGER_MAX_MESSAGE_SIZE> message = ""; ///< The current log message itself, starting from a blank slate
		LogLevel level; ///< The log level of this message

		explicit LogEntry(LogLevel level); ///< Create a new LogEntry

		/**
		 * The LogEntry destructor gets called whenever a log message is finalized, and ready to be shown to the
		 * user. This function is responsible for calling the Logger::log function.
		 *
		 * According to the C++ standard, a variable used only within an expression will be immediately destroyed once
		 * the processing of this expression is over. This allows a syntax such as `LogEntry(...) << "some" << "text"`,
		 * where the destructor will be called strictly **after** all the `<<` operations have been completed. This
		 * allows the destructor to send the finalized log entry for further processing.
		 */
		~LogEntry();

		LogEntry(LogEntry const&) = delete; ///< Unimplemented copy constructor

		/**
		 * Stream operator to append new values to a log record
		 *
		 * @tparam T The type of value to append
		 * @param value The new value to add
         * @todo See if noexcept can be added here without triggering warnings
		 * @return The current Logger::LogEntry where the value has been appended
		 */
		template <class T>
		Logger::LogEntry& operator<<(const T value) {
			etl::to_string(value, message, format, true);

			return *this;
		}

		Logger::LogEntry& operator<<(const std::string& value);
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
	static void log(LogLevel level, etl::istring & message);
};

/**
 * Create a stream to log a Message
 *
 * This functions appends one line to the Logs (which could be printed to screen, transferred via UART or stored for
 * later use.)
 *
 * Examples of usage:
 * @code
 * LOG<Logger::debug>() << "Reached point of no return";
 * LOG<Logger::error>() << "More than " << 50 << " dogs found!";
 * @endcode
 *
 * You can also use one of the \ref LOG_TRACE, \ref LOG_DEBUG, \ref LOG_INFO, \ref LOG_NOTICE, \ref LOG_WARNING,
 * \ref LOG_ERROR or \ref LOG_EMERGENCY defines, which avoid the need of explicitly passing the log level:
 * @code
 * LOG_DEBUG << "Reached point of no return";
 * LOG_ERROR << "More than " << 50 << " dogs found!";
 * @endcode
 *
 * See \ref Logger::LogLevel for an explanation of the different log levels.
 *
 * @par Implementation details
 * Functions here are defined as `constexpr` in order to let them be optimized as soon as possible. The LOG()
 * function returns an instance of \ref Logger::LogEntry if the level is high enough to be shown, or an instance of
 * \ref Logger::NoLogEntry if the log entry will not be displayed. As this is a templated function, it is acceptable
 * to support different return types using the `auto` keyword.
 *
 * @warning For messages that will not be logged, any calls to functions that contain **side effects will still take
 * place**.
 * @code
 * LOG_DEBUG << "The temperature is: " << getTemperature();
 * @endcode
 * In the above example, if `getTemperature()` will cause a side effect (e.g. an I2C connection or a `std::cout` print),
 * it will still be executed, even if the debug message will not be printed to the screen due to an insufficient
 * LOGLEVEL.
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
 * @tparam level The log level. A value of \ref Logger::LogLevel
 * @return Returns \ref Logger::LogEntry if the level is sufficient to be logged, or \ref Logger::NoLogEntry if the
 * message will not be logged. This is determined at compile-time.
 */
template <Logger::LogLevel level>
constexpr __attribute__((always_inline)) inline auto LOG() {
	if constexpr (Logger::isLogged(level)) {
		return Logger::LogEntry(level);
	} else {
		return Logger::NoLogEntry();
	}
};

/**
 * A no-op function that considers an empty log entry that will not be displayed, processed or stored.
 *
 * @warning Note that functions containing **side effects** will get properly executed. Only use functions that return
 * plain values as parts of the log function, so they might be optimized away at compile time.
 *
 * @tparam T The type of the data that will be ignored
 * @param noLogEntry A dummy no-op log entry
 * @param value The data that will be ignored
 * @return A dummy no-op log entry
 * @see Logger::LogEntry::operator<<(const T value)
 * @relates Logger::LogEntry
 */
template <typename T>
[[maybe_unused]] constexpr Logger::NoLogEntry operator<<(const Logger::NoLogEntry noLogEntry, T value) {
	return noLogEntry;
}

#endif //ECSS_SERVICES_LOGGER_HPP
