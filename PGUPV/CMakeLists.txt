

file(GLOB SRCS "src/*.cpp" "include/*.h")

add_library(PGUPV STATIC ${SRCS})

if (MSVC)
target_compile_options(PGUPV PRIVATE /W4 /WX)
endif()

target_include_directories(PGUPV PUBLIC include/)

target_link_libraries(PGUPV PRIVATE assimp::assimp freeimage nlohmann_json::nlohmann_json gli_lib)
target_link_libraries(PGUPV PRIVATE par_lib openpnp-capture SDL3_ttf::SDL3_ttf-shared)
target_link_libraries(PGUPV PRIVATE avdevice avformat avutil avcodec swscale)

target_link_libraries(PGUPV PUBLIC glm::glm GLEW guipg Microsoft.GSL::GSL)
