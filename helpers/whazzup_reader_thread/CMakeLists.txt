cmake_minimum_required(VERSION 3.15)
project(whazzup_reader_thread)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread")
include_directories(
        ../../3dparty/
        ../../common/
        ../../common/network/
        ../../simulators/xplane/xenon/
)

set(SOURCES
        main.cpp
        ../../common/network/url.cpp
        ../../3dparty/base64.cpp
        ../../simulators/xplane/xenon/ivao/ivao_periodically_updated_file.cpp
        ../../simulators/xplane/xenon/ivao/ivao_status_file.cpp
        ../../simulators/xplane/xenon/ivao/ivao_whazzup_file.cpp
        ../../simulators/xplane/xenon/ivao/whazzup_reader_thread.cpp
)
set(HEADERS
        ../../common/network/url.h
        ../../3dparty/base64.h
        ../../simulators/xplane/xenon/ivao/ivao_periodically_updated_file.h
        ../../simulators/xplane/xenon/ivao/ivao_status_file.h
        ../../simulators/xplane/xenon/ivao/ivao_whazzup_file.h
        ../../simulators/xplane/xenon/ivao/whazzup_reader_thread.h
        ../../common/settings.h
)

add_executable(whazzup_reader_thread ${SOURCES} ${HEADERS})
target_link_libraries(whazzup_reader_thread
        pthread
        boost_system
        boost_filesystem
        boost_iostreams
)
