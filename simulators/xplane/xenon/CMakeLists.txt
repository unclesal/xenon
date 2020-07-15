########################################################################################################################
# CMake file. You need set environment variable PLUGIN_DIR pointed to Resources/plugins/xenon in your X-Plane folder.
########################################################################################################################
cmake_minimum_required(VERSION 3.16)
project(xenon LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_BUILD_TYPE MATCHES "Release")

set(DISTRIB_DIR_EXISTS FALSE)
if (EXISTS "$ENV{DISTRIB_DIR}")
     set(DISTRIB_DIR_EXISTS TRUE)
     set(DISTRIB_DIR "$ENV{DISTRIB_DIR}")
endif()

# DISTRIB_DIR - это то место, куда складываем саму сгенерированную библиотеку.
# Поэтому если его нет, то это - фатально, выдаем ошибку.

if (NOT ${DISTRIB_DIR_EXISTS})
    message("Environment variable DISTRIB_DIR was not exported. Or directory \"$ENV{DISTRIB_DIR}\" does not exists.")
    set(DISTRIB_DIR ${CMAKE_SOURCE_DIR}../../../distrib/xplane_plugin_dir/xenon)
endif()

# PLUGIN_DIR - это для make install. Если его нет, то 
# это не фатально, просто не будем делать install-секцию.

set(PLUGIN_DIR_EXISTS FALSE)
if (EXISTS "$ENV{PLUGIN_DIR}")
    set(PLUGIN_DIR_EXISTS TRUE)
endif()

add_compile_options(-O3 -fPIC -Wno-deprecated-declarations)

# Enable all X-Plane SDK APIs up to the newest version.
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)

# Compilation for running inside of X-Plane
add_definitions(-DINSIDE_XPLANE)
# Produces some more messages in Log.txt file.
add_definitions(-DDEBUG)

# Define platform macros.
add_definitions(-DAPL=$<BOOL:${APPLE}> -DIBM=$<BOOL:${WIN32}> -DLIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>)

set(COMMON ../../../common)
set(NETWORK ${COMMON}/network)
set(NETCMD ${NETWORK}/commands)
set(CONTROL ${COMMON}/control)

########################################################################################################################
#                                                                                                                      #
#                                                      Source Files                                                    #
#                                                                                                                      #
########################################################################################################################

add_library(xenon MODULE
        ${COMMON}/jsonable.cpp
        ${COMMON}/abstract_aircraft.cpp
        ${COMMON}/abstract_vehicle.cpp
        ${COMMON}/bimbo_aircraft.cpp
        ${COMMON}/flight_plan.cpp
        
        ${COMMON}/airport.cpp
        ${COMMON}/airport_network.cpp
                
        ${NETCMD}/cmd_aircraft_condition.cpp
        ${NETCMD}/cmd_query_around.cpp
        ${NETCMD}/cmd_vehicle_condition.cpp
        ${NETCMD}/cmd_flight_plan.cpp
        ${NETCMD}/cmd_waypoint_reached.cpp
        
        ${NETWORK}/abstract_command.cpp
        ${NETWORK}/Ambient.cpp
        ${NETWORK}/CommandSet.cpp
        ${NETWORK}/command_parser.cpp
        ${NETWORK}/CommandAutopilotState.cpp
        ${NETWORK}/connected_communicator.cpp
        
        # Управление. 
        ${CONTROL}/aircraft_abstract_action.cpp
        ${CONTROL}/aircraft_abstract_state.cpp 
        ${CONTROL}/aircraft_does_becoming.cpp
        ${CONTROL}/aircraft_does_flying.cpp
        ${CONTROL}/aircraft_does_landing.cpp
        ${CONTROL}/aircraft_does_gliding.cpp
        ${CONTROL}/aircraft_does_lining_up.cpp
        ${CONTROL}/aircraft_does_nothing.cpp
        ${CONTROL}/aircraft_does_parking.cpp
        ${CONTROL}/aircraft_does_push_back.cpp
        ${CONTROL}/aircraft_does_runway_leaving.cpp
        ${CONTROL}/aircraft_does_slow_taxing.cpp        
        ${CONTROL}/aircraft_does_take_off.cpp
        ${CONTROL}/aircraft_does_taxing.cpp
        ${CONTROL}/aircraft_does_taxing_stop.cpp
        ${CONTROL}/aircraft_does_waiting_push_back.cpp
        ${CONTROL}/aircraft_does_waiting_take_off_approval.cpp
        ${CONTROL}/aircraft_state_airborned.cpp
        ${CONTROL}/aircraft_state_approach.cpp
        ${CONTROL}/aircraft_state_before_parking.cpp
        ${CONTROL}/aircraft_state_graph.cpp
        ${CONTROL}/aircraft_state_landed.cpp        
        ${CONTROL}/aircraft_state_parking.cpp
        ${CONTROL}/aircraft_state_on_final.cpp
        ${CONTROL}/aircraft_state_on_hp.cpp
        ${CONTROL}/aircraft_state_on_the_fly.cpp
        ${CONTROL}/aircraft_state_ready_for_take_off.cpp
        ${CONTROL}/aircraft_state_ready_for_taxing.cpp
        ${CONTROL}/aircraft_state_runway_leaved.cpp
        
        # Основной каталог.        
        main_plugin.cpp
        user_aircraft.cpp
        windll_main.cpp
        XAmbient.cpp        
        xplane_plugin.cpp
)

# C/C++ standard.
set(CMAKE_CXX_STANDARD 20)
target_compile_features(xenon PUBLIC cxx_std_20)
set_property(TARGET xenon PROPERTY CXX_STANDARD_REQUIRED 20)
set_property(TARGET xenon PROPERTY CXX_STANDARD 20)

target_include_directories(xenon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    3dparty/libXPMP2/include
    ../XPSDK301/CHeaders/XPLM
    ${COMMON}
    ${NETWORK}
    ${NETCMD}
    ${CONTROL}
    ../../../3dparty
    ../../../3dparty/nlohmann
)

target_link_libraries(xenon
    ${CMAKE_SOURCE_DIR}/3dparty/libXPMP2/libXPMP2.a
    /usr/local/lib/libGeographic.a
)


set_target_properties(xenon PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DISTRIB_DIR})
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
