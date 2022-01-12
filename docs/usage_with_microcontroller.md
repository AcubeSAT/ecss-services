# Usage with a microcontroller

@tableofcontents

ecss-services relies on many functions that may not be readily available to a microcontroller, such as printing,
getting the time, transmitting packets etc. That's why you will have to implement a few simple functions by yourself
when porting this library to a microcontroller.

## Examples

Some people have already integrated this library with their own code:
- [AcubeSAT OBC Software](https://gitlab.com/acubesat/obc/obc-software), integration with ATSAMV71Q21 and FreeRTOS
- [AcubeSAT FDIR Thesis](https://github.com/kongr45gpen/fdir-demo), integration with ATSAMV71Q21 and FreeRTOS
- [AcubeSAT OBC Mockup](https://gitlab.com/acubesat/obc/mockup-4), integration with STM32L4S9 and FreeRTOS

## The Platform directory

ecss-services declares some basic functions, but does not implement them. You, the user of this library, must write the
code for these functions that will make them work on your platform.

The `inc/Platform` and `src/Platform` directories contain some platform-specific code that has already been prepared.
The **`x86`** subdirectory includes all the code necessary to run on a Linux platform, i.e. your computer. This code is
only used when building for a desktop environment, and is not compiled when building for a microcontroller.

## Functions to implement

The following sections list and explain the functions that need to be implemented by library users. It is enough to
place the function body inside a compiled `.cpp` file. Otherwise, the linker will show errors about undefined functions.

For more details on the requirements, arguments and return values of each function, refer to their respective documentation.

### Logger

The logger is responsible for outputting messages through a serial interface used for debugging purposes.

You need to define the following function:
```cpp
void Logger::log(Logger::LogLevel level, etl::istring & message);
```

This function is responsible for getting a log level and log message, and outputting them through a serial port that can
be inspected by the developers. It is suggested to add any extra information that is available, e.g. current time or
execution thread.

An example definition can be as follows:
```cpp
void Logger::log(Logger::LogLevel level, etl::istring &message) {
    etl::string<20> time;
    etl::to_string(MCU_Get_Time(), time);

    etl::string<250> output;
    output.append(time);
    output.append(" ");
    output.append(message);

    MCU_Serial_Write(output);
}
```

### Message transmission

Whenever PUS telemetry is generated, it needs to be transmitted or sent to a receiver. This is the responsibility of the
@ref Service::storeMessage function.

In this function, you can transmit the message via an antenna, send it through an interface for debugging, or both.

An example definition can be as follows:
```cpp
void Service::storeMessage(Message& message) {
	message.finalize();

	MCU_Antenna_Transmit(message.data, message.dataSize);

	LOG_DEBUG << "Just sent ST[" << static_cast<int>(message.serviceType) << "] message";
}
```

### Error handling

The @ref ErrorHandler::logError is responsible for logging errors for debugging purposes.

@note Any generated errors are already transmitted to the ground segment via @ref Service::storeMessage. The @ref
ErrorHandler::logError function is used strictly for debugging purposes, and can be left empty if desired.

An example definition can be as follows:
```cpp
template <typename ErrorType>
void ErrorHandler::logError(const Message& message, ErrorType errorType) {
	LOG_ERROR << "Error in ["
        << static_cast<uint16_t>(message.serviceType)
        << ", " << static_cast<uint16_t>(message.messageType)
        << "]: " << errorType;
}

template <typename ErrorType>
void ErrorHandler::logError(ErrorType errorType) {
	LOG_ERROR << "Error: " << errorType;
}
```

### Service initialisation

Platform-specific code also gives a chance to every Service to use pre-initialised entities (e.g. parameters, monitoring
definitions etc.) during boot. The following functions are called at initialisation:
1. @ref ParameterService::initializeParameterMap

An example definition can be as follows:
```cpp
Parameter<uint8_t> parameter1(200);
Parameter<uint8_t> parameter2(150);
Parameter<uint8_t> parameter3(100);

void ParameterService::initializeParameterMap() {
	parameters = {{0, parameter1}, {1, parameter2}, {2, parameter3}};
}
```
