file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    snd_null
    ${CFILE}
    ${HFILE}
    )
