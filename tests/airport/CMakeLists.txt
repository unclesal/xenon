cmake_minimum_required(VERSION 3.16)
project(airport LANGUAGES C CXX)

find_package(LEMON REQUIRED)

include_directories(
    ../../common
    ../../simulators/xplane/xenon
)

set(CMAKE_CXX_STANDARD 20)

add_executable(airport )

target_sources(airport PUBLIC
    main.cpp
    ../../common/utils.cpp
    ../../simulators/xplane/xenon/airport.cpp
    ../../simulators/xplane/xenon/airport_network.cpp
#    ../../simulators/xplane/xenon/exceptions.h
)

if (LEMON_FOUND)
    include_directories(${LEMON_INCLUDE_DIRS})
    # target_link_libraries (airport ${LEMON_LIBRARIES})
endif (LEMON_FOUND)
