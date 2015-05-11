cmake_minimum_required(VERSION 2.8)

project(ASYNC_SERVER)

find_package(Boost REQUIRED system thread)
find_package(Threads)

set(COMMON_FLAGS "-Wall -std=c++0x -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

include_directories(. ${Boost_INCLUDE_DIRS})

add_executable(
	async_daemon
	async_daemon.cpp
	AsyncDaemon.cpp AsyncDaemon.h
)
target_link_libraries(
	async_daemon
	${Boost_LIBRARIES}
	cppnetlib-server-parsers
	pthread
)

