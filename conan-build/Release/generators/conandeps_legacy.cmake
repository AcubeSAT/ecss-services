message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(logger)
find_package(etl)
find_package(Catch2)

set(CONANDEPS_LEGACY  logger::logger  etl::etl  Catch2::Catch2WithMain )