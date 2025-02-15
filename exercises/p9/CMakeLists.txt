cmake_minimum_required(VERSION 3.5)

project(p09)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag" "*.tese" "*.tesc")

add_executable(p09 ${SOURCES})

target_link_libraries(p09 PGUPV)

config_exercise(p09)
