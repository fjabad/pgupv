cmake_minimum_required(VERSION 3.6...3.14)

project(p02)

# enable FetchContent
include(FetchContent)

# declare Catch2
FetchContent_Declare(pugixml                              # name of the content
  GIT_REPOSITORY https://github.com/zeux/pugixml  # the repository
  GIT_TAG        v1.14                                 # the tag
  )

# make available
FetchContent_MakeAvailable(pugixml)


file (GLOB SOURCES "*.cpp" "*.h" "*.hpp")

add_executable(p02 ${SOURCES})

target_include_directories(p02 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p02 pugixml)


config_exercise(p02)

