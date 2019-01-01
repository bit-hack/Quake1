file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    net
    ${CFILE}
    ${HFILE}
    )
