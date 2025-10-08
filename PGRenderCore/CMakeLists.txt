cmake_minimum_required(VERSION 3.16)

# Nombre del proyecto y versión
project(PGRenderCore VERSION 1.0 LANGUAGES CXX)

# Opciones recomendadas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Recopilar todos los headers en el directorio include/
file(GLOB_RECURSE PGRenderCore_SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/PGRenderCore/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(PGRenderCore STATIC ${PGRenderCore_SRC_FILES})
add_library(PGRenderCore::Core ALIAS PGRenderCore)

target_include_directories(PGRenderCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

#target_compile_definitions(PGRenderCore PUBLIC RENDERCORE_API_HEADER_ONLY=1)

