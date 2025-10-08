cmake_minimum_required(VERSION 3.16)

project(PGRenderCoreGL VERSION 1.0 LANGUAGES CXX)

# Opciones para detectar OpenGL e incluir directorios
find_package(OpenGL REQUIRED)

# Añadir directorios de código fuente
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Archivos fuente y cabeceras de PGRenderCoreGL
file(GLOB_RECURSE PGRENDERCOREGL_SOURCES
    ${SRC_DIR}/*.cpp
)

file(GLOB_RECURSE PGRENDERCOREGL_HEADERS
    ${INC_DIR}/PGRenderCoreGL/*.h
    ${INC_DIR}/PGRenderCoreGL/*.hpp
)

add_library(PGRenderCoreGL STATIC ${PGRENDERCOREGL_SOURCES} ${PGRENDERCOREGL_HEADERS})
add_library(PGRenderCore::GL ALIAS PGRenderCoreGL)

# Incluir carpetas públicas (API y backend)
target_include_directories(PGRenderCoreGL
    PUBLIC
        $<BUILD_INTERFACE:${INC_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Vincular con OpenGL
target_link_libraries(PGRenderCoreGL
    PUBLIC
        PGRenderCore OpenGL::GL
    PRIVATE
        GLEW
)

