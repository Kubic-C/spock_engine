
add_library(spock_framework STATIC)

target_include_directories(spock_framework INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(spock_framework PUBLIC
    glfw glad flecs_static glm)

target_sources(spock_framework PRIVATE
    "framework.cpp"
    "base.hpp"
    "entity.hpp" "entity.cpp"
    "opengl.hpp" "opengl.cpp"
    "window.hpp" "window.cpp"
    "debugger.hpp" "debugger.cpp"
    "memory.hpp" "memory.cpp")

target_sources(spock_framework PUBLIC
    "external/stb_image.h" "external/stb_image.c")

target_sources(spock_framework PUBLIC
    "framework.hpp")