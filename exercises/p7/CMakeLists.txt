cmake_minimum_required(VERSION 3.6...3.14)

project(p07)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag" "*.tese" "*.tesc")

add_executable(p07 ${SOURCES})

config_exercise(p07)

