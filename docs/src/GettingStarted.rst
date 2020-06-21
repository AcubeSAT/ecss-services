Getting Started
===============

This document will help you in getting a smooth ridewhile installing and
setting up the ECSS Services C++ Library

.. _1setting-up-ecss-services-c-library:

1.Setting Up ECSS Services C++ Library
--------------------------------------

The ECSS Services C++ Libraries are delivered in full source code only.
This means that you have to build the libraries and tools before you can
use them the first time.

1. Up-to-date packages are available via `Bintray`_.
2. Clone the SDK source from `ecss-services`_ on GitLab.

   -  HTTPS:``git clone https://gitlab.com/ecss-services.git``
   -  SSH: ``git clone git@gitlab.com:ecss-services.git``

.. _2external-dependencies-or-additional-requirements:

2.External Dependencies (or Additional Requirements)
----------------------------------------------------

OpenSSL
^^^^^^^

| *Unix/Linux*
| Most Unix/Linux systems already have OpenSSL preinstalled. On Ubuntu
  (or other Debian-based Linux distributions) you can type

::

   $ sudo apt-get install openssl libssl-dev

to install the necessary packages. If your system does not have OpenSSL,
please get it from `openssl.org`_

| *macOS*
| It's recommended to install OpenSSL via Homebrew.

::

    $ brew install openssl

| *Windows*
| Install via chocolatey package manager

::

   $ choco install openssl.light

.. _3building-using-cmake-or-building-the-library:

3.Building using CMake (or Building The library)
------------------------------------------------

CMake can be used to do build ECSS Services C++ Libraries. CMake is a
meta build system and it generate native makefiles and workspaces that
can be used in the compiler environment of your choice. For a quick
overview see
`cgold.readthedocs.io/en/latest/overview/cmake-can.html`_

To set up a CMake project for use with the ECSS Services

1. Create a directory to hold your source files.
   ``mkdir my_example_project``

2. Open the directory and add a ``CMakeLists.txt`` file that specifies
   your project’s name, executables, source files, and linked libraries.
   The following is a minimal example.

.. code:: CMake

           # Minimal CMakeLists.txt for the ECSS Services.
           cmake_minimum_required(VERSION 3.2)

           # Use shared libraries, which is the default for the ecss-services build.
           option(BUILD_SHARED_LIBS "Build shared libraries" ON)

           # "my-example" is just an example value.
           project(my-example)

           # Locate the ecss-services for C++ package
           find_package(ECSS_SERV REQUIRED COMPONENTS openssl)

           # The executable name and its sourcefiles.
           add_executable(my-example my-example.cpp)

           # Build using the C++ standard version 11.
           target_compile_features(my-example PUBLIC cxx_std_11)

           # The libraries used by your executable.
           target_link_libraries(my-example ${ECSS_SERV_LINK_LIBRARIES})

| Note
| You can set many options in your ``CMakeLists.txt`` build
  configuration file. For an introduction to the file’s features, see
  the `CMake tutorial`_ on the CMake website.

.. _4service-compilation-switches:

4.Service compilation Switches
------------------------------

The Library allows you to set some build time options. As an example: to
disable the services support and use only TC-TM packet support type the
following command: ENABLE_NO_SERVICES

Here is an overview of ECSS Services build options:

-  DISABLE_ST01 Set to OFF|ON (default is OFF) to build Disable Service
   ST01
-  ENABLE_TESTS Set to OFF|ON (default is OFF) to build Unit tests

Doc Notes: Link to Service Compilation Switches from doxygen

.. _Bintray: #
.. _ecss-services: #
.. _`openssl.org`: http://www.openssl.org/
.. _`cgold.readthedocs.io/en/latest/overview/cmake-can.html`: http://cgold.readthedocs.io/en/latest/overview/cmake-can.html
.. _CMake tutorial: https://cmake.org/cmake-tutorial/