
add_library(spock_framework STATIC)

target_include_directories(spock_framework INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(spock_framework PUBLIC
    glfw bgfx bimg bx flecs glm)

target_sources(spock_framework PRIVATE
    "framework.cpp"
    "base.hpp"
    "entity.hpp" "entity.cpp"
    "render.hpp" "render.cpp"
    "window.hpp" "window.cpp"
    "debugger.hpp" "debugger.cpp")

target_sources(spock_framework PUBLIC
    "framework.hpp")