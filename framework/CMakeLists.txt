
add_library(spock_framework STATIC)

target_include_directories(spock_framework INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_definitions(flecs_static PUBLIC ecs_ftime_t=double)

target_link_libraries(spock_framework PUBLIC
    SDL2 glad flecs_static glm box2d freetype)

target_sources(spock_framework PRIVATE
    "framework.cpp"
    "base.hpp"
    "entity.hpp" "entity.cpp"
    "opengl.hpp" "opengl.cpp"
    "debugger.hpp" "debugger.cpp"
    "memory.hpp" "memory.cpp")

target_sources(spock_framework PUBLIC
    "external/stb_image.h" "external/stb_image.c")

target_sources(spock_framework PUBLIC
    "framework.hpp")