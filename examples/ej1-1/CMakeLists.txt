cmake_minimum_required(VERSION 3.0)

project(ej1-1)

add_executable(ej1-1 main.cpp)
target_link_libraries(ej1-1 PGUPV)
set_target_properties(ej1-1 PROPERTIES DEBUG_POSTFIX "d")

