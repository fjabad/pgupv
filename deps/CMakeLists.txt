cmake_minimum_required(VERSION 3.14)

include(FetchContent)

find_package(OpenGL REQUIRED)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)

FetchContent_Declare(SDL
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG "main"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SDL)

if (MSVC)
add_library(freetype STATIC IMPORTED)
set (FREETYPE_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/freetype-2.13.2/objs/freetype.lib)
set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/freetype-2.13.2/include)

set_target_properties(freetype PROPERTIES
			 INTERFACE_INCLUDE_DIRECTORIES ${FREETYPE_INCLUDE_DIRS}
			 IMPORTED_IMPLIB ${FREETYPE_LIBRARY}
)

else()
endif()

set(SDL3TTF_VENDORED OFF)
FetchContent_Declare(SDL_ttf
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
    GIT_TAG "3958dc2192fcb6c32ea236c8f980131512364e95"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SDL_ttf)


FetchContent_Declare(glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG "586a402397dd35d66d7a079049856d1e2cbab300" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG "v1.90" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(imgui)


FetchContent_Declare(assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    GIT_TAG "v5.3.1" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(assimp)


FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v4.0.0"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GSL)


FetchContent_Declare(json 
    GIT_REPOSITORY "https://github.com/nlohmann/json"
    GIT_TAG "v3.11.2"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(json)


set(GLI_TEST_ENABLE OFF)
FetchContent_Declare(gli
    GIT_REPOSITORY "https://github.com/g-truc/gli.git"
#    GIT_TAG "v3.11.2"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(gli)
add_library(gli_lib INTERFACE)
target_include_directories(gli_lib INTERFACE ${gli_SOURCE_DIR})

FetchContent_Declare(par
    GIT_REPOSITORY "https://github.com/prideout/par.git"
#    GIT_TAG "v3.11.2"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(par)
add_library(par_lib INTERFACE)
target_include_directories(par_lib INTERFACE ${par_SOURCE_DIR})


if (MSVC)
set (DIRENT_BUILD_TESTS OFF)
FetchContent_Declare(dirent
    GIT_REPOSITORY "https://github.com/tronkko/dirent.git"
#    GIT_TAG "v3.11.2"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(dirent)

add_library(dirent_lib INTERFACE)
target_include_directories(dirent_lib INTERFACE ${dirent_SOURCE_DIR}/include)
set(EXTRA_LIBS "dirent_lib")



set(FreeImageDLL ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/FreeImage.dll)
add_library(freeimage SHARED IMPORTED)
set_target_properties(freeimage PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/
        IMPORTED_LOCATION ${FreeImageDLL}
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/FreeImage.lib
)


set(FFmpeg_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-6.1-full_build-shared")
set(FFmpeg_INCLUDE_DIR ${FFmpeg_ROOT}/include)
set(FFmpeg_BIN_DIR ${FFmpeg_ROOT}/bin)

file(GLOB FF_LIBS ${FFmpeg_ROOT}/lib/*.lib)

foreach (LIBFILE ${FF_LIBS})
get_filename_component(module ${LIBFILE} NAME_WE)
set(libname lib${module})
message("Building " ${libname})

# find the dll
file(GLOB DLL ${FFmpeg_BIN_DIR}/${module}*.dll)

add_library(${libname} SHARED IMPORTED)
set_target_properties(${libname} PROPERTIES
			 INTERFACE_INCLUDE_DIRECTORIES ${FFmpeg_INCLUDE_DIR}
             IMPORTED_LOCATION ${DLL}
			 IMPORTED_IMPLIB ${LIBFILE}
)
set(EXTRA_LIBS ${EXTRA_LIBS} ${libname})
endforeach()


else()
find_package(FREEIMAGE REQUIRED)

find_package(FFMPEG)
set(EXTRA_LIBS ${FFMPEG_LIBRARIES})
endif()

add_library(PG-deps INTERFACE)
target_link_libraries(PG-deps INTERFACE SDL3::SDL3 assimp::assimp SDL3_ttf::SDL3_ttf freeimage 
	nlohmann_json::nlohmann_json gli_lib ${EXTRA_LIBS} par_lib)

add_library(PG-public-deps INTERFACE)
target_link_libraries(PG-deps INTERFACE glm::glm)

add_library(glew STATIC glew-2.2.0/src/glew.c)
target_include_directories(glew PUBLIC glew-2.2.0/include)
target_compile_definitions(glew PUBLIC GLEW_STATIC)
target_link_libraries(glew PUBLIC OpenGL::GL)


set(imdir ${imgui_SOURCE_DIR})

add_library(dearimgui STATIC ${imdir}/imgui.cpp ${imdir}/imgui_draw.cpp ${imdir}/imgui_tables.cpp ${imdir}/imgui_widgets.cpp ${imdir}/backends/imgui_impl_sdl3.cpp ${imdir}/backends/imgui_impl_opengl3.cpp)
target_include_directories(dearimgui PUBLIC ${imdir}/ ${imdir}/backends)
target_compile_definitions(dearimgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW GLM_ENABLE_EXPERIMENTAL IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUIZMO_IMGUI_FOLDER= IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(dearimgui PRIVATE SDL3::SDL3)
