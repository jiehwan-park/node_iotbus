CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT("node_iotbus")

INCLUDE(FindPkgConfig)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_C_FLAGS_PROFILING    "-O2")
SET(CMAKE_CXX_FLAGS_PROFILING  "-O2 -std=c++0x")
SET(CMAKE_C_FLAGS_DEBUG        "-O0 -g")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -std=c++0x -g")
SET(CMAKE_C_FLAGS_RELEASE      "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -std=c++0x -g -fvisibility-inlines-hidden")
SET(CMAKE_CXX_FLAGS_CCOV       "-O0 -std=c++0x -g --coverage")

ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Wall")                # Generate all warnings
ADD_DEFINITIONS("-Wextra")              # Generate even more extra warnings
ADD_DEFINITIONS("-Wno-variadic-macros") # Inhibit variadic macros warnings (needed for ORM)
ADD_DEFINITIONS("-Wno-deprecated")      # No warnings about deprecated features
ADD_DEFINITIONS("-std=c++0x")           # accept C++11x standard
ADD_DEFINITIONS("-fvisibility=hidden")

SET(GLOBAL_NODE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/node)
INCLUDE(build/common.cmake)

ADD_SUBDIRECTORY(modules)
