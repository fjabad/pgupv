cmake_minimum_required(VERSION 3.6...3.14)

project(ej5-2)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag")

add_executable(ej5-2 ${SOURCES})

config_example(ej5-2)