#ifndef ECSS_SERVICES_MACROS_HPP
#define ECSS_SERVICES_MACROS_HPP

/**
 * Perform an assertion that, if failed, throws an ErrorHandler::Internal error
 *
 * @todo Actually hold program execution or throw an exception here
 */
#define assertI(cond, error) (ErrorHandler::assertInternal((cond), (error)))

/**
 * A wrapper for ErrorHandler::assertRequest() that uses `this` as the Message object.
 *
 * Only to be used within the Message class.
 */
#define assertR(cond, error) (ErrorHandler::assertRequest((cond), *this, (error)))

#endif //ECSS_SERVICES_MACROS_HPP
