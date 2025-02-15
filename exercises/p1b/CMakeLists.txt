cmake_minimum_required(VERSION 3.5)

project(p01b)

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

add_executable(p01b ${SOURCES})

target_link_libraries(p01b PGUPV pugixml)


config_exercise(p01b)

