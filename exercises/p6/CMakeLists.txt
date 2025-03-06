cmake_minimum_required(VERSION 3.6...3.14)

project(p06)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag")

add_executable(p06 ${SOURCES})

target_link_libraries(p06 PGUPV)

config_exercise(p06)

