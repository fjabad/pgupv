cmake_minimum_required(VERSION 3.6...3.14)

project(p05)

file (GLOB SOURCES "*.cpp" "*.h" "*.hpp" "*.vert" "*.frag")

add_executable(p05 ${SOURCES})

config_exercise(p05)