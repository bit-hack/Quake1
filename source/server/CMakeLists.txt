file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    server
    ${CFILE}
    ${HFILE}
    )
