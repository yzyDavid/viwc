cmake_minimum_required(VERSION 3.3)
project(vic)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Wall")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_C_COMPILER clang)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_C_COMPILER gcc)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

set(SOURCE_FILES main.c
        file_str.c
        file_str.h
        draw_ui.c
        draw_ui.h
        file_sl.c
        file_sl.h
        intmouse.c
        intmouse.h
        m_loop.c
        m_loop.h
        main.h
        n_loop.c
        n_loop.h
        ins_loop.c
        ins_loop.h
        highlit.c
        highlit.h
        log_md.c
        log_md.h
        btm_loop.c
        btm_loop.h)

add_executable(vic ${SOURCE_FILES})

