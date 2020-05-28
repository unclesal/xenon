cmake_minimum_required(VERSION 3.15)
project(airport LANGUAGES C CXX)

# set(GeographicLib_DIR /usr/local)
# find_package (GeographicLib REQUIRED)

include_directories(
    /usr/local/include
    ../../3dparty
    ../../common
    ../../simulators/xplane/xenon
)

set(CMAKE_CXX_STANDARD 20)

add_executable(airport )

target_sources(airport PUBLIC
    main.cpp
    ../../simulators/xplane/xenon/airport.cpp
    ../../simulators/xplane/xenon/airport_network.cpp
    ../../simulators/xplane/xenon/xplane.hpp
#    ../../simulators/xplane/xenon/exceptions.h
)

target_link_libraries ( airport /usr/local/lib/libGeographic.a )

#if (LEMON_FOUND)
#    include_directories(${LEMON_INCLUDE_DIRS})
#    # target_link_libraries (airport ${LEMON_LIBRARIES})
#endif (LEMON_FOUND)
