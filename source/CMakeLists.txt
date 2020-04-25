add_subdirectory(common)
add_subdirectory(progs)
add_subdirectory(server)
add_subdirectory(net)


add_subdirectory(in_sdl)
add_subdirectory(snd_null)
add_subdirectory(snd_sdl)

file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

find_package(SDL REQUIRED)

include_directories(SYSTEM ${SDL_INCLUDE_DIR})

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
    net
    snd_sdl
    in_sdl
    OPENGL32.lib
    ws2_32.lib
    winmm.lib
    Comctl32.lib
    ${SDL_LIBRARY}
    )
