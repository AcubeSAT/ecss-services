# Overview to the PUS standard {#pus-overview}

## Trivia
- Version C of the standard contains 656 pages, often filled with verbose requirements and difficult definitions. ???
- This standard is not optimised for heavy communication, e.g. real-time audio/video, large files, etc.
- This repository _does not implement the entire standard_. A subset has been selected that should be useful to our
  mission, or to any CubeSat that requires some basic functionality. ECSS characteristically mentions that this
  standard serves as a _"menu"_ from which to select all relevant services.
- While all services are designed in a modular manner, there are often various relationships between them (e.g. ST[05]
  event reporting — ST[19] event-action). Additionally, some services provide _observable_ parameters (e.g. ST[23] may
  provide the available memory space as an observable)
- Each service can be implemented by a single microcontroller in a spacecraft, or by multiple different units. The
  standard does not define any implementation restrictions on this regard.
