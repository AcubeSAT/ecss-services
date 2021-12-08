# Logger

To use this as a submodule, run `git submodule add https://gitlab.com/acubesat/obc/logger ECSS-logger`.
If you are using this in an already existing on-board software AcubeSAT repository, it is very likely that you have a `lib/` directory; it is advised to add the `logger` submodule inside `lib/`.
If you use this as a submodule, maybe you won't have to grab the [ETL](https://github.com/ETLCPP/etl) submodule from here, and use the ETL folder from the parent instead.
If not, make sure to `git submodule update --recursive` first.
Your `CMakeLists.txt` would have then have a line like `include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/lib/etl/include" "${PROJECT_SOURCE_DIR}/lib/ECSS-logger/inc")`
This will receive a more elegant solution in the future, perhaps in the form of a package manager.
