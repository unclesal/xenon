########################################################################################################################
# CMake file. You need set environment variable PLUGIN_DIR pointed to Resources/plugins/xenon in your X-Plane folder.
########################################################################################################################
cmake_minimum_required(VERSION 3.9)
project(xenon LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_BUILD_TYPE MATCHES "Release")

add_compile_options(-O3 -fPIC)

# Enable all X-Plane SDK APIs up to the newest version.
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)

# Compilation for running inside of X-Plane
add_definitions(-DINTERNAL_XPLANE)
# Produces some more messages in Log.txt file.
add_definitions(-DDEBUG)

# Define platform macros.
add_definitions(-DAPL=$<BOOL:${APPLE}> -DIBM=$<BOOL:${WIN32}> -DLIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>)

########################################################################################################################
#                                              Source Files
########################################################################################################################
add_library(xenon MODULE
        # xenon sources
        ../../../common/network/Ambient.cpp
        ../../../common/network/CommandSet.cpp
        ../../../common/network/CommandAutopilotState.cpp
        ../../../common/jsonable.cpp
        ../../../common/utils.cpp
        abstract_aircraft.cpp
        abstract_vehicle.cpp
        airport.cpp
        bimbo_aircraft.cpp
        main_plugin.cpp
        user_aircraft.cpp
        windll_main.cpp
        XAmbient.cpp
        XNetworking.cpp
        xplane_plugin.cpp
)

# C/C++ standard.
set(CMAKE_CXX_STANDARD 20)
target_compile_features(xenon PUBLIC cxx_std_20)
set_property(TARGET xenon PROPERTY CXX_STANDARD_REQUIRED 20)
set_property(TARGET xenon PROPERTY CXX_STANDARD 20)

target_include_directories(xenon PUBLIC
        3dparty/libXPMP2/include
        ../XPSDK301/CHeaders/XPLM
        ../../../common
        ../../../common/network
        ../../../3dparty
        ../../../3dparty/nlohmann
)

target_link_libraries(xenon ${CMAKE_SOURCE_DIR}/3dparty/libXPMP2/libXPMP2.a )

message("Plugin dir is" $ENV{PLUGIN_DIR})

set_target_properties(xenon PROPERTIES LIBRARY_OUTPUT_DIRECTORY "$ENV{PLUGIN_DIR}")
set_target_properties(xenon
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME "lin"
    SUFFIX ".xpl"
)
