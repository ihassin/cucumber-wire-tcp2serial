cmake_minimum_required(VERSION 3.2)
project(wire_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        api.c
        api.h
        context.h
        handle-request.c
        handle-request.h
        main.c
        system-includes.h
        token-handling.c
        token-handling.h
        wire-listener.c
        wire-listener.h
        wire-server.c
        wire-server.h
        scenario.c
        scenario.h
        serial.h
        serial.c)

add_executable(wire_server ${SOURCE_FILES})