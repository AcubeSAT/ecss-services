# Installation

@tableofcontents

The ECSS Services library was is developed and tested on Linux. It is possible to use Mac OS or Windows, if the relevant
tools are installed.

## Required software {#required-software}
Before getting started, you will need to make sure that you have the required compilation tools installed. We use the
[CMake](https://cmake.org/) build system for all the instrumentation required for compiling.

On Debian-based systems, a command like the following should suffice:
```bash
sudo apt install git cmake make binutils build-essential
```

If you want to run the **optional tools**, including documentation generators and code checkers, you will need to
install them as well:
```bash
sudo apt install doxygen clang-tidy clang-format gcovr lcov cppcheck
```

## Compiling {#compiling}
To download and build the ecss-services repo, use the following terminal commands:

1. Clone the repository, along with its submodules
   ```bash
   git clone --recursive https://gitlab.com/acubesat/obc/ecss-services.git
   ```

2. Create the `build` directory. This is where the executables and outputs will be stored.
   ```bash
   cd ecss-services
   mkdir build
   ```

3. Create the CMake project and Makefile
   ```bash
   cmake ..
   ```

4. Run the build!
   ```bash
   make -j${nproc}
   ```

You will be able to run the `./ecss-services` script to see how the output of the services may look like, or `./tests`
to run some unit tests and verify that the code works correctly.

## Other tasks {#other}

Apart from compiling the services, you can also generate other outputs if you want.

### Integrating with your code {#integrating}
While ecss-services can run independently, the repo is built as a library. Follow the instructions in
[Usage with a microcontroller](docs/usage_with_microcontroller.md) to integrate with your space or ground segment code.


### Creating the documentation {#documentation}
To create the documentation files, make sure you have `doxygen` installed. Afterwords, you can just run from the root
directory:
```bash
doxygen
```

### Running static analysis scripts {#static-analysis}
To run the static analysis scripts, you can open `.gitlab-ci.yml` and find the script you are interested in. Make sure
to have the relevant static analysis tool installed beforehand!
