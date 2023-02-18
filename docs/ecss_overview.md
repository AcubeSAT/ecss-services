# Overview to the PUS standard {#pus-overview}

@tableofcontents

The `ecss-services` library implements the ECSS-E-ST-70-41C **Telemetry and telecommand packet utilization** standard,
released by the European Cooperation for Space Standardization. The complete text of the standard is
[available after a free registration](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/).
This page summarises the core concepts of the standard, and how it can be useful to a typical space mission.

The ECSS-E-ST-70-41C standard (also called **Packet Utilisation Standard**, or **PUS** for short) lists a number of
**services** that a spacecraft can support. These services define basic operational functions of a spacecraft, such as
file storage, statistics generation, responding to events and many others. Each Service is also accompanied with
different Message types. These messages can be:
- **Telecommand (TC) requests**: Messages from a Ground Station to the Spacecraft. These messages often request specific
  information, or command the spacecraft to take specific actions.
- **Telemetry (TM) reports**: These can be responses to other commands, or they can inform the Ground Station about
  interesting events and information happening on the spacecraft.

The standard clearly defines the **byte-level structure** of each Message, as well as the **capabilities** that each
  Service provides.

## Important entities
A few entities defined in ECSS-E-ST-70-41C are used throughout the standard and are useful to keep in mind:
1. **@ref Parameter "Parameters"**. A parameter is a value (often numerical) that represents a small piece of data which
   can be sent to or received from the satellite. Parameters can represent sensor outputs, configuration values, status
   indicators, or everything else.

   Parameters are mainly handled by the @ref ParameterService, but they are regularly used throughout the code when
   dealing with satellite data.

   Parameters can be numbers of different types, strings or (rarely) more complex entities.
2. **Events**. Events represent expected or unexpected occurences on the spacecraft.

   The @ref EventReportService is mainly responsible for management of on-board events. Other services provide the
   capability of generating or responding to on-board events.

3. **Application Processes** (AP). An Application Process is any physical (hardware) or logical (software) entity that
   can handle PUS messages.

   In most cases, an Application Process will be a single microcontroller or subsystem. For example, the
   <span title="On-Board Computer">OBC</span>, <span title="Attitude Determination and Control Subsystem">ADCS</span> and Ground
   Station may be different Application Processes.

   Typically, a message will have an AP as a source, and another AP as its destination. For example, if the Ground
   Station wants to send a message to the OBC, it can send a Telecommand where the OBC Application Process is listed as
   the destination.

   @todo Application Process handling is not yet implemented in this library. See
   [#55](https://gitlab.com/acubesat/obc/ecss-services/-/issues/55).

## The services

This section provides a very short overview of each ECSS service, and how it can be typically used in a space mission.

All services are designed to be **highly configurable during flight**. While they can have a pre-defined configuration
at launch, the Ground Station can send simple telecommands (TC) to modify the services.

The standard aptly defines itself as a _"menu"_ from which services are chosen. The following services do not need to be
fully implemented for each mission, but need to be _tailored_ according to the requirements and needs of the spacecraft
and its environment.

Each service is further divided into some **subservices** that are logical groupings of a service's functionality. A
large space mission may split every subservice into different parts of the hardware. However, this implementation _makes
no distinction between subservices_.

### Standard's Services

<b>`ST[01]`: Request verification</b>

Provides acknowledgement or failure reports for executed commands. This service essentially informs the operators about
the status of TCs sent to the spacecraft, and reports any occurred errors during parsing or execution.

See @ref #error-handling for more information regarding error handling on this library.

<b>`ST[02]`: Device access</b>

Allows toggling, controlling and configuring on-board peripherals that rely on simple protocols to communicate
(such as I2C).

<b>`ST[03]`: Housekeeping</b>

Produces periodic TM reports with values of parameters. This service is the one providing the most essential information
about the status of the spacecraft.

ST[03] is also highly configurable during flight. For example, it allows:
- Selecting which parameters we want to be periodically transmitted
- How often the parameters should be transmitted
- Enabling/disabling specific types of periodic reports at will

<b>`ST[04]`: Parameter statistics reporting</b>

Allows reporting statistics (min, max, mean, standard deviation) for specific parameters over specified durations. This
is a data-efficient alternative to the `ST[03]` housekeeping service.

<b>`ST[05]`: Event reporting</b>

Generates reports when notable occurrences take place on-board, such as:

- Autonomous on-board actions
- Detected failures or anomalies
- Predefined steps during an operation

<b>`ST[06]`: Memory management</b>

Allows writing and reading directly from an on-board memory unit. This can be useful for debugging and investigative
purposes, fetching mission data, or uploading new software to the spacecraft's memories. The service also allows
downlinking and uplinking files in a file system.

<b>`ST[08]`: Function management</b>

Provides the capability of running predefined actions that can receive further parameters. These actions can correspond
to payload, platform, or any other functionality.

<b>`ST[09]`: Time management</b>

Allows periodic reporting of the current spacecraft time for observability and correlation purposes.

<b>`ST[11]`: Time-based scheduling</b>

Allows the operators to <b>"time-tag"</b> telecommands for execution at future timestamps, instead of immediately. Its use
is essential when communication with your spacecraft is not guaranteed for the entirety of its orbit.

This is one of the most complicated services to use, as it allows complex functionality such as:
- Running a command after some duration of time
- Running a command at a specified time
- Running a command after successful execution of another command
- Running a command only if another command fails

<b>`ST[12]`: On-board monitoring</b>

This service allows checking parameter values to ensure that they remain within configurable limits. Whenever a
violation occurs, an `ST[05]` event can be optionally generated for further processing.

<b>`ST[13]`: Large packet transfer</b>

Provides a method of message segmentation, for message payloads that are too large to fit within the maximum allowed
length for TC or TM.

The maximum limits are usually imposed by the lower communication layers. For example, the Communications subsystem of a
satellite may use a transceiver that can only send packets up to 64 bytes long.

<b>`ST[14]`: Real-time forwarding control</b>

This service is responsible of controlling which types of generated reports are immediately transmitted to the ground.

<b>`ST[15]`: On-board storage and retrieval</b>

Allows storing generated TM reports on-board.

If your spacecraft does not have constant communication with the Ground Station (e.g. a low-earth orbit satellite), it
will be necessary to store all messages until they can be downlinked safely. The ST[15] service can be used to:
- Downlink all packets that haven't yet been transmitted
- Filter packets by their type
- Filter packets by the time of their creation

<b>`ST[17]`: Test</b>

Allows performing on-board connection checks, similar to "pinging" a machine and asking if it is alive.

<b>`ST[18]`: On-board operations procedure</b>

Allows loading, controlling (start, suspend, resume, abort) and configuring On-Board Control Procedures, which are
sequences of commands written in an application-specific language.

<b>`ST[19]`: Event-action</b>

Provides the capability of automatically executing arbitrary actions when an `ST[05]` event is triggered. This service
can be an essential component when planning the autonomous functionalities of a spacecraft.

<b>`ST[20]`: On-board parameter management</b>

Provides the capability of reading and setting on-board parameters.

<b>`ST[21]`: Request sequencing</b>

Allows operators to load series of TCs to be executed in a sequential order.

<b>`ST[22]`: Position-based scheduling</b>

Provides the capability of executing TCs when the spacecraft reaches a specific point in its orbit.

<b>`ST[23]`: File management</b>

Provides the capability of managing on-board file systems, with functions such as *copy*, *move*, *delete*, or *create
directory*.

### Custom Services

<b>`ST[128]`: Dummy Service</b>

Provides the capability of storing log strings as ECSS Messages.

## Trivia
- Version C of the standard contains 656 pages, often filled with verbose requirements and difficult definitions. Every
  requirement is identified by its requirement ID. [Version
  A](https://ecss.nl/standard/ecss-e-70-41a-ground-systems-and-operations-telemetry-and-telecommand-packet-utilization/)
  contains a simpler but outdated and less rigorous version of the standard.
- This standard is not optimised for heavy communication, e.g. real-time audio/video, large files, etc. Other protocols
  and solutions can be used for this purpose (e.g. [CFDP](https://public.ccsds.org/Pubs/720x1g4.pdf)).
- While the standard is mainly designed for ground-space communication, it can theoretically be used to coordinate
  communication between different subsytems onboard a spacecraft.
- This repository _does not implement the entire standard_. A subset has been selected that should be useful to our
  mission, or to any CubeSat that requires some basic functionality. ECSS characteristically mentions that this standard
  serves as a "menu" from which to select all relevant services.
- While all services are designed in a modular manner, there are often various relationships between them (e.g. ST[05]
  event reporting — ST[19] event-action). Additionally, some services provide _observable_ parameters (e.g. ST[23] may
  provide the available memory space as an observable)
- Each service can be implemented by a single microcontroller in a spacecraft, or by multiple different units. The
  standard does not define any implementation restrictions on this regard.
