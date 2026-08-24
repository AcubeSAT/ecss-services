#ifndef ECSS_SERVICES_MACROS_HPP
#define ECSS_SERVICES_MACROS_HPP

/**
 * Perform an assertion that, if failed, throws an ErrorHandler::Internal error
 * and logs the source filename and line.
 *
 * @todo (#134) Actually hold program execution or throw an exception here
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_INTERNAL(cond, error) (ErrorHandler::assertInternal((cond), (error), __FILE__, __LINE__))

/**
 * A wrapper for ErrorHandler::assertRequest() that uses `this` as the Message object.
 *
 * Only to be used within the Message class.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_REQUEST(cond, error) (ErrorHandler::assertRequest((cond), *this, (error)))

#endif // ECSS_SERVICES_MACROS_HPP
