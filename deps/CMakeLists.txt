cmake_minimum_required(VERSION 3.14)

include(FetchContent)

find_package(OpenGL REQUIRED)


set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SDL_TEST OFF)
set(SDL_TEST_LIBRARY OFF)

set(BUILD_SHARED_LIBS ON)

FetchContent_Declare(SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG "release-3.2.14"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SDL3)

set(SDL3TTF_VENDORED OFF)
set(SDL3TTF_INSTALL OFF)
set(SDL3TTF_SAMPLES OFF)
FetchContent_Declare(SDL3_ttf
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
    GIT_TAG "release-3.2.2"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SDL3_ttf)


set(GLM_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
#    GIT_TAG "1.0.1" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG "v1.91.9b" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(imgui)


FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v4.0.0"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GSL)


FetchContent_Declare(json 
    GIT_REPOSITORY "https://github.com/nlohmann/json"
    GIT_TAG "v3.11.3"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(json)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(GLI_TEST_ENABLE OFF)
add_subdirectory(gli-master)
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

set(WITH_JPEG8 ON)
add_subdirectory(openpnp-capture)
# set the include dir
target_include_directories(openpnp-capture PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/openpnp-capture/include>)


set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
FetchContent_Declare(assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    GIT_TAG "v5.4.3" 
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(assimp)


# enable FetchContent
include(FetchContent)

FetchContent_Declare(pugixml                              # name of the content
  GIT_REPOSITORY https://github.com/zeux/pugixml  # the repository
  GIT_TAG        v1.14                                 # the tag
  )

FetchContent_MakeAvailable(pugixml)

if (MSVC)
add_library(GLEW STATIC glew-2.2.0/src/glew.c)
target_include_directories(GLEW PUBLIC glew-2.2.0/include)
target_compile_definitions(GLEW PUBLIC GLEW_STATIC)
target_link_libraries(GLEW PUBLIC OpenGL::GL)

add_library(freeimage SHARED IMPORTED GLOBAL)
set_target_properties(freeimage PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/FreeImage.dll
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/FreeImage/Dist/x64/FreeImage.lib
)


set(FFmpeg_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-6.1-full_build-shared")
set(FFmpeg_INCLUDE_DIR ${FFmpeg_ROOT}/include)

file(GLOB FF_LIBS ${FFmpeg_ROOT}/lib/*.lib)

foreach (LIBFILE ${FF_LIBS})
	get_filename_component(module ${LIBFILE} NAME_WE)
	set(libname lib${module})

	# find the dll
	file(GLOB DLL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${module}*.dll)

	add_library(${libname} SHARED IMPORTED GLOBAL)
	set_target_properties(${libname} PROPERTIES
				 INTERFACE_INCLUDE_DIRECTORIES ${FFmpeg_INCLUDE_DIR}
				 IMPORTED_LOCATION ${DLL}
				 IMPORTED_IMPLIB ${LIBFILE}
	)
endforeach()
else()
find_package(FREEIMAGE REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(GLEW REQUIRED)
endif()

set(imdir ${imgui_SOURCE_DIR})

add_library(dearimgui STATIC ${imdir}/imgui.cpp ${imdir}/imgui_draw.cpp ${imdir}/imgui_tables.cpp ${imdir}/imgui_widgets.cpp ${imdir}/backends/imgui_impl_sdl3.cpp ${imdir}/backends/imgui_impl_opengl3.cpp)
target_include_directories(dearimgui PUBLIC ${imdir}/ ${imdir}/backends)
target_compile_definitions(dearimgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW GLM_ENABLE_EXPERIMENTAL IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUIZMO_IMGUI_FOLDER= IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(dearimgui PRIVATE SDL3::SDL3)

if (MSVC)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(assimp dearimgui freetype gli_dummy glm GLEW PROPERTIES FOLDER "deps")
set_target_properties(openpnp-capture harfbuzz plutosvg plutovg SDL_uclibc PROPERTIES FOLDER "deps")
set_target_properties(SDL3_ttf-shared SDL3-shared PROPERTIES FOLDER "deps")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs zlibstatic pugixml-static PROPERTIES FOLDER "deps")
endif()


