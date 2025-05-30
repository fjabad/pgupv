if (NOT MSVC) 
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
find_package(SDL3 REQUIRED)
endif()

find_package(OpenGL REQUIRED)

set(COMMON_FILES nfd_common.c guipg.cpp icons.cpp imGuIZMOquat.cpp logConsole.cpp)

if(MSVC) 
add_library (guipg STATIC ${COMMON_FILES} nfd_win.cpp)
else()
add_library (guipg STATIC ${COMMON_FILES} nfd_gtk.c)
endif()

target_include_directories(guipg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${GTK3_INCLUDE_DIRS} )
target_link_libraries(guipg PRIVATE ${GTK3_LIBRARIES} OpenGL::GL SDL3::SDL3 dearimgui glm::glm Microsoft.GSL::GSL)




