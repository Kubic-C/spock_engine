
add_library(spock_engine STATIC)

target_link_libraries(spock_engine PUBLIC spock_framework)
target_include_directories(spock_engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "engine.cpp"
    "systems.hpp" "systems.cpp"
    "scene.hpp" "scene.cpp")

target_sources(spock_engine PRIVATE
    "systems/renderer.hpp" "systems/renderer.cpp"
    "systems/primitive_renderer.hpp" "systems/primitive_renderer.cpp"
    "systems/physics.hpp" "systems/physics.cpp"
    "systems/input.hpp" "systems/input.cpp")

target_sources(spock_engine PRIVATE
    "utility/physics.hpp" "utility/physics.cpp"
    "utility/aabb.hpp" "utility/aabb.cpp"
    "utility/batching.hpp" "utility/batching.cpp"
    "utility/callback.hpp" "utility/callback.cpp")

target_sources(spock_engine PUBLIC
    "engine.hpp")