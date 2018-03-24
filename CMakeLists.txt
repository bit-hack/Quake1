cmake_minimum_required(VERSION 3.0)
project(fitzquake)

file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

set(USE_SDL FALSE CACHE BOOL "Use SDL sybsystem")

if(USE_SDL)
    list(REMOVE_ITEM CFILE
        "${CMAKE_CURRENT_SOURCE_DIR}/sys_win.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/gl_vid_win.c"
        )
else()
    list(REMOVE_ITEM CFILE
        "${CMAKE_CURRENT_SOURCE_DIR}/sys_sdl.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/gl_vid_sdl.c"
        )
endif()

add_executable(
    fitzquake
    WIN32
    ${CFILE}
    ${HFILE}
    )

target_link_libraries(
    fitzquake
    OPENGL32.lib
    ws2_32.lib
    winmm.lib
    Comctl32.lib
    )
