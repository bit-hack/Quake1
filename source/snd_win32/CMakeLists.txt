file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_library(
    snd_win32
#    SHARED
    ${CFILE}
    ${HFILE}
    )

target_link_libraries(snd_win32 winmm.lib)