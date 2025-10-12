cmake_minimum_required(VERSION 3.20)

project(PGRenderCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_subdirectory(deps)
add_subdirectory(guipg)

add_subdirectory(PGUPV)
add_subdirectory(examples)
add_subdirectory(exercises)



# Opciones para seleccionar backends a compilar
option(PGRENDERCORE_BUILD_OPENGL "Build OpenGL 4.x backend" ON)
option(PGRENDERCORE_BUILD_VULKAN "Build Vulkan backend" OFF)
option(PGRENDERCORE_BUILD_METAL "Build Metal backend" OFF)
option(PGRENDERCORE_BUILD_DIRECTX12 "Build DirectX 12 backend" OFF)
option(PGRENDERCORE_BUILD_FACTORY "Build unified factory library" ON)
option(PGRENDERCORE_BUILD_EXAMPLES "Build example applications" OFF)

# Buscar dependencias comunes
find_package(OpenGL)
if(PGRENDERCORE_BUILD_OPENGL AND NOT OPENGL_FOUND)
    message(WARNING "OpenGL not found, disabling OpenGL backend")
    set(PGRENDERCORE_BUILD_OPENGL OFF)
endif()

# Añadir subdirectorios
add_subdirectory(PGRenderCore)

if(PGRENDERCORE_BUILD_OPENGL)
    add_subdirectory(PGRenderCoreGL)
endif()

if(PGRENDERCORE_BUILD_VULKAN)
    add_subdirectory(PGRenderCoreVulkan)
endif()

if(PGRENDERCORE_BUILD_METAL)
    add_subdirectory(PGRenderCoreMetal)
endif()

if(PGRENDERCORE_BUILD_DIRECTX12)
    add_subdirectory(PGRenderCoreD3D12)
endif()

if(PGRENDERCORE_BUILD_FACTORY)
    add_subdirectory(PGRenderCoreFactory)
endif()

if(PGRENDERCORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(PGRenderCoreApp)
