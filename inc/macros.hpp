#ifndef ECSS_SERVICES_MACROS_HPP
#define ECSS_SERVICES_MACROS_HPP

#define assertI(cond, error) (ErrorHandler::assertInternal((cond), (error)))
#define assertR(cond, error) (ErrorHandler::assertRequest((cond), *this, (error)))

#endif //ECSS_SERVICES_MACROS_HPP
