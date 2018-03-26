file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    snd_sdl
    ${CFILE}
    ${HFILE}
    )
