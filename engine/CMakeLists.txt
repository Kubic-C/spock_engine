
add_library(spock_engine STATIC)

target_link_libraries(spock_engine PUBLIC spock_framework)
target_include_directories(spock_engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "spock.cpp"
    "systems.hpp" "systems.cpp"
    "state.hpp" "state.cpp"
    "events.hpp" "events.cpp")

target_sources(spock_engine PRIVATE
    "systems/window.hpp" "systems/window.cpp"
    "systems/render.hpp" "systems/render.cpp"
    "systems/physics.hpp" "systems/physics.cpp"
    "systems/primitive_render.hpp" "systems/primitive_render.cpp"
    "systems/ps_tracker.hpp" "systems/ps_tracker.cpp"
    "systems/ui.hpp" "systems/ui.cpp")

target_sources(spock_engine PRIVATE
    "components/window.hpp" "components/window.cpp"
    "components/box2D.hpp" "components/box2D.cpp"
    "components/ui.hpp" "components/ui.cpp")

target_sources(spock_engine PRIVATE
    "utility/ui_.hpp" "utility/ui_.cpp"
    "utility/text.hpp" "utility/text.cpp"
    "utility/resource_manager.hpp" "utility/resource_manager.cpp")

target_sources(spock_engine PUBLIC
    "spock.hpp")