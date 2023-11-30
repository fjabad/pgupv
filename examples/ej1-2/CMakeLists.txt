cmake_minimum_required(VERSION 3.5)

project(ej1-2)

add_executable(ej1-2 main.cpp)
target_link_libraries(ej1-2 PGUPV)
set_target_properties(ej1-2 PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET ej1-2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")