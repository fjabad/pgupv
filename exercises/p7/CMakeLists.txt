cmake_minimum_required(VERSION 3.5)

project(p07)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag" "*.tese" "*.tesc")

add_executable(p07 ${SOURCES})

target_link_libraries(p07 PGUPV)

config_exercise(p07)

