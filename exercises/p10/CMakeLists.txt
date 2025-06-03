cmake_minimum_required(VERSION 3.6...3.14)

project(p10)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag" "*.tese" "*.tesc")

add_executable(p10 ${SOURCES})

config_exercise(p10)
