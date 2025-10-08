cmake_minimum_required(VERSION 3.16)

project(PGRenderCoreFactory VERSION 1.0 LANGUAGES CXX)

# Recopilar archivos fuente
# Añadir directorios de código fuente
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/PGRenderCoreFactory)

# Archivos fuente y cabeceras de PGRenderCoreFactory
file(GLOB_RECURSE PGRENDERCOREFACTORY_SOURCES
    ${SRC_DIR}/*.cpp
)

file(GLOB_RECURSE PGRENDERCOREFACTORY_HEADERS
    ${INC_DIR}/*.h
    ${INC_DIR}/*.hpp
)

# Crear librería
add_library(PGRenderCoreFactory STATIC ${PGRENDERCOREFACTORY_SOURCES} ${PGRENDERCOREFACTORY_HEADERS})
add_library(PGRenderCore::Factory ALIAS PGRenderCoreFactory)

target_include_directories(PGRenderCoreFactory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencia obligatoria de la base
target_link_libraries(PGRenderCoreFactory
    PUBLIC
        PGRenderCore::Core
)

# Enlazar backends habilitados
if(PGRENDERCORE_BUILD_OPENGL)
    target_link_libraries(PGRenderCoreFactory PUBLIC PGRenderCore::GL)
    target_compile_definitions(PGRenderCoreFactory PRIVATE PGRENDERCORE_OPENGL_SUPPORT)
endif()

if(PGRENDERCORE_BUILD_VULKAN)
    target_link_libraries(PGRenderCoreFactory PUBLIC PGRenderCore::Vulkan)
    target_compile_definitions(PGRenderCoreFactory PRIVATE PGRENDERCORE_VULKAN_SUPPORT)
endif()

if(PGRENDERCORE_BUILD_METAL)
    target_link_libraries(PGRenderCoreFactory PUBLIC PGRenderCore::Metal)
    target_compile_definitions(PGRenderCoreFactory PRIVATE PGRENDERCORE_METAL_SUPPORT)
endif()

if(PGRENDERCORE_BUILD_DIRECTX12)
    target_link_libraries(PGRenderCoreFactory PUBLIC PGRenderCore::D3D12)
    target_compile_definitions(PGRenderCoreFactory PRIVATE PGRENDERCORE_D3D12_SUPPORT)
endif()

