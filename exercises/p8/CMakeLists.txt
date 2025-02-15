cmake_minimum_required(VERSION 3.5)

project(p08)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag" "*.tese" "*.tesc")

add_executable(p08 ${SOURCES})

target_link_libraries(p08 PGUPV)

config_exercise(p08)

