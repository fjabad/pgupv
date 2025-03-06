cmake_minimum_required(VERSION 3.6...3.14)

project(p04)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag")

add_executable(p04 ${SOURCES})

target_link_libraries(p04 PGUPV)

config_exercise(p04)

