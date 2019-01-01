file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    in_win
    ${CFILE}
    ${HFILE}
    )
