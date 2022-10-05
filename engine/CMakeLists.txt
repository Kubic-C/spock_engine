
add_library(spock_engine STATIC)

target_link_libraries(spock_engine PUBLIC spock_framework)
target_include_directories(spock_engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "engine.cpp"
    "systems.hpp" "systems.cpp"
    "input.hpp" "input.cpp"
    "scene.hpp" "scene.cpp")

target_sources(spock_engine PRIVATE
    "systems/renderer.hpp" "systems/renderer.cpp"
    "systems/collider_renderer.hpp" "systems/collider_renderer.cpp"
    "systems/ui_renderer.hpp" "systems/ui_renderer.cpp"
    "systems/ui.hpp" "systems/ui.cpp" 
    "systems/physics.hpp" "systems/physics.cpp")

target_sources(spock_engine PRIVATE
    "utility/physics_.hpp" "utility/physics_.cpp"
    "utility/ui_.hpp" "utility/ui_.cpp"
    "utility/callback.hpp" "utility/callback.cpp"
    "utility/text.hpp" "utility/text.cpp"
    "utility/resource_manager.hpp" "utility/resource_manager.cpp")

target_sources(spock_engine PUBLIC
    "engine.hpp")