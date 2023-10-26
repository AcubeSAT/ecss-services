# Usage with a microcontroller {#usage-mcu}

@tableofcontents

ecss-services relies on many functions that may not be readily available to a microcontroller, such as printing,
getting the time, transmitting packets etc. That's why you will have to implement a few simple functions by yourself
when porting this library to a microcontroller.

The library is also attempting to be modular enough to support the needs of your own spacecraft. After setting it up,
you will be able to specify your own parameters, functions, events, and other definitions. All limits and numeric
constants are not hard-coded, but can be modified in @ref ECSSDefinitions.

@attention The ecss-services repository is not designed to be used as a static or dynamic library. Due to the embedded
nature of the project, ecss-services may have a different binary output for every different usecase. You will need to
compile this library independently for different projects.

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

## Integration with CMake

While `ecss-services` is a typical C++ project, it is built and based around the [CMake build
system](https://cmake.org/). If you are using CMake in your project, you can easily integrate `ecss-services` in your
`CMakeLists.txt` file, just by adding these lines:

```cmake
# ecss-services include directories
include_directories(lib/ecss-services/inc)
include_directories(lib/ecss-services/inc/Platform/x86)
include_directories(lib/ecss-services/lib/etl/include)
include_directories(lib/ecss-services/lib/logger/inc)

# Build ecss-services and link it to the project
add_subdirectory(lib/ecss-services EXCLUDE_FROM_ALL)
target_link_libraries(<project_name> common)
```

Assuming your project is called `<project_name>` and the library is installed in `lib/ecss-services`, the above lines
will automatically compile ecss-services for you using the compiler applicable to your application, and link them to
your project.

## Functions to implement

The following sections list and explain the functions that need to be implemented by library users. It is enough to
place the function body inside a compiled `.cpp` file. Otherwise, the linker will show errors about undefined functions.

For more details on the requirements, arguments and return values of each function, refer to their respective
documentation.

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

#### Setting the log level

You will also need to set the **minimum log level** of your application by setting the relevant `LOGLEVEL` constants.

For example, you can add this to your `CMakeLists.txt` to log all messages:

```cmake
add_compile_definitions(LOGLEVEL_TRACE)
```

For a list of all possible log levels, refer to the documentation of the Logger.

@note If you want to have different log levels for different parts of your application, you can
use [`target_compile_definitions`](https://cmake.org/cmake/help/latest/command/target_compile_definitions.html).

@note All logs with a level lower than the specified one will not be compiled at all, and will not be included in any
form in the resulting binary. This means that disabled log messages _will not_ have any negative impact on the
performance and size of your program.

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

### Using the Timer

A significant portion of the PUS functionalities uses timestamps, for example in order to compare message reception times,
or reporting event times as telemetry. However, time tracking is performed by the hardware, so the time-related functions
existing in the Services should be re-implemented, as the current functions contain dummy values. The **TimeGetter** class
is responsible for giving access to a Real-Time Clock, enabling the above capabilities.

#### Getting the current UTC time

The **getCurrentTimeUTC** function computes the current UTC time. In order to actually acquire the current time,
modify the arguments (which are dummy by default), so that they correspond to their real values. An example is given
below.

```cpp
UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	UTCTimestamp currentTime(onBoardYear, onBoardMonth, onBoardDay, onBoardHour, onBoardMinute, onBoardSecond);
	return currentTime;
}
```

## Service initialisation

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
2. @ref TimeBasedSchedulingService::notifyNewActivityAddition

The TimeBasedSchedulingService is responsible for storing the activities/TCs to be executed. However, if this list of 
stored activities is emptied, the Platform Task responsible to execute these TCs should be paused. With this 
function, a notifier can be implemented and called by `TimeBasedScheduling::insertActivities()` to notify the 
Task to restart after a new activity is inserted.

An example implementation for FreeRTOS can be as follows:
```cpp
void TimeBasedSchedulingService::notifyNewActivityAddition() {
    if (scheduledActivities.size() >= 1) {
        xTaskNotify(TaskList::timeBasedSchedulingTask->taskHandle, 0, eNoAction);
    }
}
```

3. @ref ParameterStatisticsService::initializeStatisticsMap

The function is called by the constructor of the class, making sure that the statistics will be initialized properly
after creating an instance of the ST04 Service. This function should be implemented according to the specific
needs, structure and parameters of an individual project. It basically iterates over the statistics list and initializes
statistics for parameters of selected IDs. An example is provided below. Let's assume that we have stored the
parameter IDs, and we use a data structure to store the statistics. Our initialization function, based on such
structure should look like this:

```c++
namespace PlatformParameters {
    enum ParameterIDs : uint16_t {
        OnBoardMinute = 0,
        Temperature = 1,
    };
}

namespace ParameterStatistics {
	static etl::array<Statistic, ECSSMaxStatisticParameters> statistics = {
	    Statistic(),
	    Statistic(),
	};
}

void ParameterStatisticsService::initializeStatisticsMap() {
	using namespace PlatformParameters;
	uint16_t statisticParameterIDs[] = {parameterIDs::onBoardMinute, parameterIDs::temperature};
	uint8_t idIndex = 0;

    for (auto& statistic: ParameterStatistics::statistics) {
        statisticsMap.insert({statisticParameterIDs[idIndex++], statistic});
    }
}
```

## Receiving messages

After making sure that your code compiles, you need to provide a way of feeding received TC into the services. This can
be done easily:

```cpp
MessageParser::parse(string, size);
```

You can use the rest of the @ref MessageParser functions if you have a more specific use-case.
