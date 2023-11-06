# Installation

@tableofcontents

The ECSS Services library was is developed and tested on Linux. It is possible to use Mac OS or Windows, if the relevant
tools are installed.

## Required software {#required-software}
Before getting started, you will need to make sure that you have the required compilation tools installed. We use the
[CMake](https://cmake.org/) build system for all the instrumentation required for compiling, and [Conan](https://conan.io/)
for dependency management.

On Debian-based systems, a command like the following should suffice:
```bash
sudo apt install git cmake make binutils build-essential
sudo pip install conan
```

We recommend CMake >= 3.23 and Conan >= 2.0.  
Follow the instructions in https://apt.kitware.com/ to install the latest CMake version on a Debian-based system.

If you want to run the **optional tools**, including documentation generators and code checkers, you will need to
install them as well:
```bash
sudo apt install doxygen clang-tidy clang-format gcovr lcov cppcheck
```

## Compiling {#compiling}

This is a CMake project with dependencies managed by Conan. The [typical instructions](https://docs.conan.io/2/tutorial/consuming_packages/build_simple_cmake_project.html)
for similar projects will work here as well.

### Step 1: Clone the repository

The first step is to clone the repository:
```bash
git clone https://gitlab.com/acubesat/obc/ecss-services.git
```

The main build does not require any submodules. However, building the documentation will require some extra CSS
that you can include by running:
```bash
git submodule update --init --recursive
```

### Step 2: Create a conan profile

If you don't have a conan profile already, you can create one by running:
```bash
conan profile detect
```

This will create a conan file for your system, specifying a compiler and an architecture for the build.
You can edit it if you prefer to cross-compile for a different architecture.

### Step 3: Download dependencies

The following command will download all the dependencies through conan:
```bash
conan install . --output-folder=build --build=missing --setings=build_type=Debug
```

You can change the `output-folder` based on your preference, or if you want to have multiple different builds.
This will prepare the output folder for the build outputs.

The `build_type` can be one of conan's build types, i.e. `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`.

You can specify add the `--update` flag to update the dependencies to the latest version.

@note
    `conan install` will only download the dependencies, without building anything. You can use `conan build`
    instead to automatically build the entire project, completing the next two steps as well.


@note
    If you need to download a private package, you may need to log in with your details in the private conan
    repository:
    ```bash
    conan remote add artifactory https://artifactory.spacedot.gr/artifactory/api/conan/conan
    conan remote login conan $YOUR_USERNAME
    ```


@note
    If you not have access to our private artifactory, you can clone the dependencies on your own, and package them
    by running `conan create . --build=missing`

### Step 4: Prepare the CMake project

Conan will create a `CMakeUserPresets.json` that contains a link to a `conan_toolchain.cmake`.
This file prepares the CMake project to use the compiler and dependencies defined by conan.

To use it, you can either select a CMake build preset (Cmake >= 3.23):
```bash
cmake -B build --preset=conan-debug
```

Or, you can use the `conan_toolchain.cmake` directly:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

Make sure to change `debug` to the appropriate option for your desired build type.

You can specify any other options you wish to CMake through this step, for example
`-DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic"` to show more warnings.

### Step 5: Build the project

ecss-services uses Unix Makefiles by default, so you can build the project by running:
```bash
cd build && make -j8
```

The number `8` specifies the number of concurrently running jobs, usually the number of CPU cores.
You can use `nproc` to get the number of processing units on your system.

CMake can also run all the necessary steps to build the project for you, by running:
```bash
cmake --build build
```

### Step 6: Execute and test

CMake will place the completed outputs in the `build/{build_type}` directory. You can run the
executables and libraries generated in there.



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
