cmake_minimum_required(VERSION 3.0)
project(fitzquake)

file(GLOB CFILE *.c)
file(GLOB HFILE *.h)

add_executable(fitzquake WIN32 ${CFILE} ${HFILE})
target_link_libraries(fitzquake OPENGL32.lib ws2_32.lib winmm.lib Comctl32.lib dinput8.lib)
