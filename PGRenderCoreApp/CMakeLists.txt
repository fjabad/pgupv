cmake_minimum_required(VERSION 3.20)

# Opciones de compilación
option(USE_OPENGL "Use OpenGL backend" ON)
option(USE_VULKAN "Use Vulkan backend" OFF)

# ===== BUSCAR DEPENDENCIAS =====

# Buscar SDL3 (para gestión de ventanas)
#find_package(SDL3::SDL3 REQUIRED)

# ===== ARCHIVOS FUENTE =====

file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE APP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

# ===== CREAR EJECUTABLE =====

add_executable(PGRenderCoreApp 
    ${APP_SOURCES}
    ${APP_HEADERS}
)

# ===== DIRECTORIOS DE INCLUDES =====

target_include_directories(PGRenderCoreApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ===== ENLAZAR LIBRERÍAS =====

target_link_libraries(PGRenderCoreApp
    PRIVATE
        PGRenderCore::Factory  # Usar la factory para conveniencia
        SDL3::SDL3
)

# Si prefieres usar backend específico en lugar de factory:
# target_link_libraries(PGRenderCoreApp
#     PRIVATE
#         PGRenderCore::Core
#         PGRenderCore::GL
#         ${SDL_TARGET}
# )

# ===== CONFIGURACIÓN DE COMPILACIÓN =====

# Añadir flags de optimización en Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(PGRenderCoreApp PRIVATE /O2 /W4)
    else()
        target_compile_options(PGRenderCoreApp PRIVATE -O3 -Wall -Wextra)
    endif()
endif()

# ===== COPIAR RECURSOS (opcional) =====

# Copiar carpeta de shaders al directorio de salida
add_custom_command(TARGET PGRenderCoreApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:PGRenderCoreApp>/assets
    COMMENT "Copying assets to output directory"
)


# ===== INFORMACIÓN DE CONFIGURACIÓN =====

message(STATUS "=================================")
message(STATUS "Project: PGRenderCoreApp")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL Backend: ${USE_OPENGL}")
message(STATUS "Vulkan Backend: ${USE_VULKAN}")
message(STATUS "=================================")
