########################################################################################################################
# CMake file. You need set environment variable PLUGIN_DIR pointed to Resources/plugins/xenon in your X-Plane folder.
########################################################################################################################
cmake_minimum_required(VERSION 3.9)
project(xenon LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_BUILD_TYPE MATCHES "Release")

set(DISTRIB_DIR_EXISTS FALSE)
if (EXISTS "$ENV{DISTRIB_DIR}")
     set(DISTRIB_DIR_EXISTS TRUE)
endif()

# DISTRIB_DIR - это то место, куда складываем саму сгенерированную библиотеку.
# Поэтому если его нет, то это - фатально, выдаем ошибку.

if (NOT ${DISTRIB_DIR_EXISTS})
    message(FATAL_ERROR "Environment variable DISTRIB_DIR was not exported. Or directory \"$ENV{DISTRIB_DIR}\" does not exists.")
endif()

# PLUGIN_DIR - это для make install. Если его нет, то 
# это не фатально, просто не будем делать install-секцию.

set(PLUGIN_DIR_EXISTS FALSE)
if (EXISTS "$ENV{PLUGIN_DIR}")
    set(PLUGIN_DIR_EXISTS TRUE)
endif()

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
        ../../../common/network/Ambient.cpp
        ../../../common/network/CommandSet.cpp
        ../../../common/network/CommandAutopilotState.cpp
        ../../../common/jsonable.cpp
        ../../../common/utils.cpp
        # Управление. Пока что в этом же директории, на потом надо будет перенести в common.
        control/aircraft_abstract_does.cpp
        control/aircraft_abstract_state.cpp        
        control/aircraft_does_push_back.cpp
        control/aircraft_does_slow_taxing.cpp
        control/aircraft_state_graph.cpp
        control/aircraft_state_parking.cpp
        control/aircraft_state_on_hp.cpp
        control/aircraft_state_ready_for_taxing.cpp
        # Основной каталог.
        abstract_aircraft.cpp
        abstract_vehicle.cpp
        airport.cpp
        airport_network.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}
        control
        3dparty/libXPMP2/include
        ../XPSDK301/CHeaders/XPLM
        ../../../common
        ../../../common/network
        ../../../3dparty
        ../../../3dparty/nlohmann
)

target_link_libraries(xenon ${CMAKE_SOURCE_DIR}/3dparty/libXPMP2/libXPMP2.a )

set_target_properties(xenon PROPERTIES LIBRARY_OUTPUT_DIRECTORY "$ENV{DISTRIB_DIR}")
set_target_properties(xenon
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME "lin"
    SUFFIX ".xpl"
)

# install-секция
if (${PLUGIN_DIR_EXISTS})
    message("-- Install section will be generated for $ENV{PLUGIN_DIR}")
    install(TARGETS xenon DESTINATION "$ENV{PLUGIN_DIR}")
endif()

## Копирование файлов из XPMP2
#set(XPMP2_ROOT_DIR $ENV{XPMP2_ROOT_DIR})
#set(XENON_XPMP2_DIR ${XENON_ROOT_DIR}/simulators/xplane/xenon/3dparty/libXPMP2)
#
#if (EXISTS ${XENON_ROOT_DIR})
#    add_custom_command(TARGET XPMP2 POST_BUILD
#            # copy builded library direct into xenon folder
#            COMMAND ${CMAKE_COMMAND} -E remove_directory ${XENON_XPMP2_DIR}
#            COMMAND ${CMAKE_COMMAND} -E make_directory ${XENON_XPMP2_DIR}/include
#            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/inc/ ${XENON_XPMP2_DIR}/include/
#            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:XPMP2> ${XENON_XPMP2_DIR}
#    )
#endif()