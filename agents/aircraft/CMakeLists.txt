########################################################################################################################
#                                         Сборка внешнего агента, отображающего самолет                                #
#                                                                                                                      #
# Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                           Created 27 may 2020 at 08:18 #
########################################################################################################################
cmake_minimum_required(VERSION 3.15)
project(agent_aircraft LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# set(CMAKE_BUILD_TYPE MATCHES "Release")

message("Build type: ${CMAKE_BUILD_TYPE}")
add_compile_options(-fPIC -Wno-deprecated-declarations)

# Debug build?
if(CMAKE_BUILD_TYPE MATCHES "Release")
    add_compile_options( -O3 )
endif()

# Сборка - именно для агента.
add_definitions( -DINSIDE_AGENT )

# С отладкой или без нее. Без отладки - сразу же уйдет в fork().
add_definitions( -DDEBUG )

set(COMMON ../../common)
set(CONTROL ${COMMON}/control)
set(NET ${COMMON}/network)
set(FRAMES ../frames)
set(NETCMD ${NET}/commands)

include_directories(
    ../../3dparty/
    ${COMMON}
    ${NET}
    ${NETCMD}
    ${CONTROL}
    ${FRAMES}
    ../
    ${CMAKE_SOURCE_DIR}
)

set(SOURCES 
    ${COMMON}/jsonable.cpp
    ${COMMON}/logger.cpp
    ${COMMON}/abstract_vehicle.cpp
    ${COMMON}/abstract_aircraft.cpp
    ${COMMON}/bimbo_aircraft.cpp
    ${COMMON}/airport_network.cpp
    ${COMMON}/airport.cpp
    ${COMMON}/flight_plan.cpp
    
    ${FRAMES}/abstract_frame.cpp
    ${FRAMES}/entity_frame.cpp
    ${FRAMES}/hp_lu_occupated.cpp
    ${FRAMES}/hp_some_one_landing.cpp
    ${FRAMES}/lu_before_take_off.cpp    
    ${FRAMES}/state_frame.cpp
    ${FRAMES}/push_back_allowed.cpp
    ${FRAMES}/taxing_distance.cpp   
    ${FRAMES}/taxing_push_back_ahead.cpp
    
    ${NET}/abstract_command.cpp
    ${NET}/command_parser.cpp
    ${NET}/connected_communicator.cpp
    
    ${NETCMD}/cmd_vehicle_condition.cpp
    ${NETCMD}/cmd_query_around.cpp
    ${NETCMD}/cmd_aircraft_condition.cpp
    ${NETCMD}/cmd_flight_plan.cpp
    ${NETCMD}/cmd_waypoint_reached.cpp

    ${CONTROL}/aircraft_abstract_action.cpp
    ${CONTROL}/aircraft_abstract_state.cpp
    ${CONTROL}/aircraft_does_becoming.cpp
    ${CONTROL}/aircraft_does_flying.cpp
    ${CONTROL}/aircraft_does_gliding.cpp
    ${CONTROL}/aircraft_does_landing.cpp
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
    ${CONTROL}/aircraft_state_on_final.cpp
    ${CONTROL}/aircraft_state_on_hp.cpp
    ${CONTROL}/aircraft_state_on_the_fly.cpp
    ${CONTROL}/aircraft_state_parking.cpp
    ${CONTROL}/aircraft_state_ready_for_take_off.cpp
    ${CONTROL}/aircraft_state_ready_for_taxing.cpp
    ${CONTROL}/aircraft_state_runway_leaved.cpp

    ../abstract_agent.cpp
    agent_aircraft.cpp
    main.cpp
)

# Выполняемый файл самого сервера.
add_executable(agent_aircraft ${SOURCES} main.cpp)
target_compile_features(agent_aircraft PUBLIC cxx_std_20)

target_link_libraries(agent_aircraft
    pthread
    /usr/local/lib/libGeographic.a
)

