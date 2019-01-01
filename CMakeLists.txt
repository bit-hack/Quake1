cmake_minimum_required(VERSION 3.0)
project(fitzquake)

add_subdirectory(common)
add_subdirectory(progs)
add_subdirectory(server)

add_subdirectory(snd_win32)
add_subdirectory(snd_null)
add_subdirectory(snd_sdl)

file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

set(USE_SDL FALSE CACHE BOOL "Use SDL sybsystem")

if(USE_SDL)

    find_package(SDL REQUIRED)
    
    include_directories(SYSTEM ${SDL_INCLUDE_DIR})
    target_include_directories(snd_win32 SYSTEM PUBLIC ${SDL_INCLUDE_DIR})

    list(REMOVE_ITEM CFILE
        "${CMAKE_CURRENT_SOURCE_DIR}/sys_win.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/gl_vid_win.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/in_win.c"
        )
else()
    list(REMOVE_ITEM CFILE
        "${CMAKE_CURRENT_SOURCE_DIR}/sys_sdl.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/gl_vid_sdl.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/in_sdl.c"
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
    common
    progs
    server
    #snd_null
    #snd_win32
    snd_sdl
    OPENGL32.lib
    ws2_32.lib
    winmm.lib
    Comctl32.lib
    ${SDL_LIBRARY}
    )
