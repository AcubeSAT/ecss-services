# AcubeSAT ECSS Services {#mainpage}

This repository contains the implementation of the [ECSS-E-ST-70-41C](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/) standard, as used in the AcubeSAT mission.

## Installation

Read [Installation](docs/installation.md) to see how to download, compile and install ecss-services.

Read [Usage with Microcontroller](docs/usage_with_microcontroller.md) to see how to integrate ecss-services with your
satellite or embedded system.

## How to read this documentation

This documentation contains the technical documentation for the implementation of the ECSS documentation. For a description of the requirements see [version A](https://ecss.nl/standard/ecss-e-70-41a-ground-systems-and-operations-telemetry-and-telecommand-packet-utilization/) and [version C](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/) of the standard.

To get into the documentation you can read:
1. A list of [pages](pages.html) that contain generic information on the philosophy, design and usage of the services
2. A list of [modules](modules.html) that group interesting functionality with the relevant instructions
3. A list of [classes](annotated.html) that contain the most interesting entities and objects in the code
4. A list of [namespaces](namespaces.html) that contain grouped utility functions or variables

A good starting point would be the @ref Services module. You can browse through each ECSS Service based on the functionality you desire.

