cmake_minimum_required(VERSION 2.8)

project(ASYNC_SERVER)

find_package(Boost REQUIRED system thread)
find_package(CURL)
find_package(Threads)

set(COMMON_FLAGS "-Wall -std=c++0x -pthread -DBUILD_TYPE=${CMAKE_BUILD_TYPE}")

set(DEBUG_FLAGS "-DDEBUG=DEBUG")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")

include_directories(. ${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(
	async_daemon
	async_daemon.cpp
	AsyncDaemon.cpp AsyncDaemon.h
)
target_link_libraries(async_daemon ${Boost_LIBRARIES} cppnetlib-server-parsers)

if (UNIX AND NOT APPLE)
	target_link_libraries(async_daemon pthread rt)
endif ()

