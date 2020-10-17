cmake_minimum_required(VERSION 3.17)
project(SocketIRC C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_1 server)
set(SOURCE_FILES_1 src/Server.c src/lib/cmd.h src/lib/cprofile.h)
add_executable(${BUILD_1} ${SOURCE_FILES_1})

set(BUILD_2 client)
set(SOURCE_FILES_2 src/Client.c src/lib/cmd.h src/lib/cprofile.h src/test.c src/lib/cpool.h)
add_executable(${BUILD_2} ${SOURCE_FILES_2})