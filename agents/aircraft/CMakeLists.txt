########################################################################################################################
#                                         Сборка внешнего агента, отображающего самолет                                #
#                                                                                                                      #
# Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                           Created 27 may 2020 at 08:18 #
########################################################################################################################
cmake_minimum_required(VERSION 3.17)
project(agent_aircraft LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_BUILD_TYPE MATCHES "Release")

add_compile_options(-O3 -fPIC -Wno-deprecated-declarations)

# Сборка - именно для серверной части.
add_definitions(-DINSIDE_AGENT)

# С отладкой или без нее. Без отладки - сразу же уйдет в fork().
add_definitions(-DDEBUG)

set(PDIR ../../simulators/xplane/xenon/)

include_directories(
    ${PDIR}
    ${PDIR}/control/
    ../../3dparty/
    ../../common/
    ../../common/network/
    ../../common/network/commands/
    ../
    ${CMAKE_SOURCE_DIR}
)

set(SOURCES 
    ../../common/jsonable.cpp
    ../../common/network/commands/cmd_hello.cpp
    ../../common/network/abstract_command.cpp
    ../../common/network/command_parser.cpp
    ../../common/network/connected_communicator.cpp
    ../airport_network.cpp
    ../airport.cpp
    ../abstract_agent.cpp
    ${PDIR}/abstract_vehicle.cpp
    ${PDIR}/abstract_aircraft.cpp
    ${PDIR}/bimbo_aircraft.cpp
    ${PDIR}/control/abstract_aircrafter.cpp
    ${PDIR}/control/aircraft_abstract_action.cpp
    ${PDIR}/control/aircraft_abstract_state.cpp
    ${PDIR}/control/aircraft_does_flying.cpp
    ${PDIR}/control/aircraft_does_landing.cpp
    ${PDIR}/control/aircraft_does_lining_up.cpp
    ${PDIR}/control/aircraft_does_nothing.cpp
    ${PDIR}/control/aircraft_does_push_back.cpp
    ${PDIR}/control/aircraft_does_runway_leaving.cpp
    ${PDIR}/control/aircraft_does_slow_taxing.cpp
    ${PDIR}/control/aircraft_does_take_off.cpp
    ${PDIR}/control/aircraft_does_taxing.cpp
    ${PDIR}/control/aircraft_state_airborned.cpp
    ${PDIR}/control/aircraft_state_graph.cpp
    ${PDIR}/control/aircraft_state_on_final.cpp
    ${PDIR}/control/aircraft_state_on_hp.cpp
    ${PDIR}/control/aircraft_state_parking.cpp
    ${PDIR}/control/aircraft_state_ready_for_take_off.cpp
    ${PDIR}/control/aircraft_state_ready_for_taxing.cpp
    ${PDIR}/control/aircraft_state_runway_leaved.cpp
    agent_aircraft.cpp
    external_aircraft.cpp
    main.cpp
)

# Выполняемый файл самого сервера.
add_executable(agent_aircraft ${SOURCES} main.cpp)
target_compile_features(agent_aircraft PUBLIC cxx_std_20)

target_link_libraries(agent_aircraft
    pthread
    /usr/local/lib/libGeographic.a
)

