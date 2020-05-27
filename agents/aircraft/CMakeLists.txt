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

include_directories(
    ../../3dparty/
    ../../common/
    ../../common/network/
    ../../common/network/commands/
    ../    
)

set(SOURCES 
    ../../common/jsonable.cpp
    ../../common/network/commands/cmd_hello.cpp
    ../../common/network/abstract_command.cpp
    ../../common/network/command_parser.cpp
    ../../common/network/connected_communicator.cpp
    ../abstract_agent.cpp
    agent_aircraft.cpp
    main.cpp
)

# Выполняемый файл самого сервера.
add_executable(agent_aircraft ${SOURCES} main.cpp)
target_link_libraries(agent_aircraft pthread)
target_compile_features(agent_aircraft PUBLIC cxx_std_20)
