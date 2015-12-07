cmake_minimum_required(VERSION 3.3)
project(cucumber_tcp2serial)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        wire-server/api.c
        wire-server/api.h
        wire-server/context.h
        wire-server/handle-request.c
        wire-server/handle-request.h
        wire-server/main.c
        wire-server/system-includes.h
        wire-server/token-handling.c
        wire-server/token-handling.h
        wire-server/wire-listener.c
        wire-server/wire-listener.h
        wire-server/wire-server.c
        wire-server/wire-server.h
        wire-server/scenario.c
        wire-server/scenario.h
        wire-server/serial.c
        wire-server/serial.h)

add_executable(cucumber_tcp2serial ${SOURCE_FILES})