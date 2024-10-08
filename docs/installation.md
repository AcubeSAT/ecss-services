# Installation

@tableofcontents

The ECSS Services library is developed and tested on Linux. It is possible to use Mac OS or Windows, if the relevant
tools are installed. WSL is also supported out of the box.

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

### Step 3: Add the SpaceDot repository

SpaceDot-specific packages are hosted on a public artifactory repository. You will need to add it to conan before you
can download our packages:
```bash
conan remote add spacedot https://artifactory.spacedot.gr/artifactory/api/conan/conan
```

@parblock
@note
If you need to download a private package or upload a new release, you may need to log in with your personal details:
```bash
conan remote login spacedot $YOUR_USERNAME # You will be prompted for a password
```
@endparblock

@parblock
@note
If, for some reason, you do not have access to our public artifactory instance, you can build our internal packages on
your own, by cloning the respective repositories, and running `conan create . --build=missing` inside them.

### Step 4: Download dependencies

The following command will download all the dependencies through conan:
```bash
conan install . --output-folder=build --build=missing --settings=build_type=Debug
```
@endparblock

You can change the `output-folder` based on your preference, or if you want to have multiple different builds.
This will prepare the output folder for the build outputs.

The `build_type` can be one of conan's build types, i.e. `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`.

You can specify add the `--update` flag to update the dependencies to the latest version.

@note `conan install` will only download the dependencies, without building anything. You can use `conan build` 
instead to automatically build the entire project, completing the next two steps as well.

### Step 5: Prepare the CMake project

Conan will create a `CMakeUserPresets.json` that contains a link to a `conan_toolchain.cmake`.
This file prepares the CMake project to use the compiler and dependencies defined by conan.

To use it, you can either select a CMake build preset (CMake >= 3.23):
```bash
cmake -B build/Debug --preset=build-debug
```

Or, you can use the `conan_toolchain.cmake` directly:
```bash
cmake -B build/Debug -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

Make sure to change `debug` to the appropriate option for your desired build type.

You can specify any other options you wish to CMake through this step, for example
`-DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic"` to show more warnings.

@note The name of the appropriate CMake preset you need to use will be shown as the output of
the `conan install` command.

### Step 6: Build the project

ecss-services uses Unix Makefiles by default, so you can build the project by running:
```bash
cd build/Debug && make -j8
```

The number `8` specifies the number of concurrently running jobs, usually the number of CPU cores.
You can use `nproc` to get the number of processing units on your system.

CMake can also run all the necessary steps to build the project for you, by running:
```bash
cmake --build build/Debug -- -j8
```

### Step 7: Execute and test

CMake will place the completed outputs in the `build/{build_type}` directory. You can run the
executables and libraries generated in there.


### Using an IDE?

If you are using an IDE that manages CMake, it should be enough to add the following option to the CMake command:
```bash
--preset=cmake-build-debug-debug
# or a different configuration (e.g. build-release)
```

@note
We do not recommend using Conan plugins for your IDE, as they might work in mysterious ways and override our custom
configuration. We cannot provide support for such plugins as our team is mainly using Conan from the command line.

## Integrating as a library

While `ecss-services` can run as a standalone executable, it is built to be used mainly as a library. Follow these 
instructions to integrate it with your own code.

@remark ecss-services is free software. You must make sure to follow the licensing restrictions from the LICENSE 
file for any usage.

The services are best used as a Conan package, since they themselves depend on other Conan packages.

### Step 0: Prepare your environment

Make sure you have your Conan profile and the project's Artifactory remote set up:
```bash
conan profile detect
conan remote add spacedot https://artifactory.spacedot.gr/artifactory/api/conan/conan
```

See the [official Conan guide](https://docs.conan.io/2/tutorial/consuming_packages/build_simple_cmake_project.html) 
on how to set up a project whose dependencies are managed by Conan.

### Step 1: Add the package to your dependencies

Edit your `conanfile.txt` or `conanfile.py` files to add the following dependency:
```
[requires]
ecss-services/1.1
```

You can see more information about the available versions of the package at
https://artifactory.spacedot.gr/ui/packages/?name=ecss-services&type=packages.

### Step 2: Integrate with CMake

Include and link the `common` library in your CMake project. This should include and link the required files with 
your executable.

```
find_package(ecss-services CONFIG REQUIRED COMPONENTS common)
# ...
target_link_libraries(my_target common)
```

### Concurrent Developing

If you want to develop both a parent project and its dependency at the same time, you should use [Conan editable 
packages](https://docs.conan.io/2/tutorial/developing_packages/editable_packages.html). These allow your repository 
to build directly from the source code, instead of downloading a pre-made package.

First, you will need to download `ecss-services` independently from your project (e.g. through Git).
Then, you need to mark it as an editable package:
```bash
conan editable add /path/to/ecss-services --output-folder /path/to/ecss-services/build
```

Make sure that you have built `ecss-services` before moving on:
```bash
conan build /path/to/ecss-services --output-folder=build --build=missing --setings=build_type=Debug
```

Then, your project should be compileable directly:
```bash
conan build /path/to/my/project --build=missing --setings=build_type=Debug # And any other settings you prefer
```

@note The build type between the two builds from the two projects must be the same.

After you have finished developing, you can remove the editable package:
```bash
conan editable remove /path/to/ecss-services
```

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
