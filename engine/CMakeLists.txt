
add_library(spock_engine STATIC)

target_link_libraries(spock_engine PUBLIC spock_framework)
target_include_directories(spock_engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "spock.cpp"
    "base.hpp"
    "systems.hpp" "systems.cpp"
    "state.hpp" "state.cpp"
    "events.hpp" "events.cpp"
    "debug.hpp" "debug.cpp")

target_sources(spock_engine PRIVATE
    "systems/window.hpp" "systems/window.cpp"
    "systems/render.hpp" "systems/render.cpp"
    "systems/physics.hpp" "systems/physics.cpp"
    "systems/primitive_render.hpp" "systems/primitive_render.cpp"
    "systems/sprite_render.hpp" "systems/sprite_render.cpp"
    "systems/ps_tracker.hpp" "systems/ps_tracker.cpp"
    "systems/ui.hpp" "systems/ui.cpp"
    "systems/camera.hpp" "systems/camera.cpp")

target_sources(spock_engine PRIVATE
    "components/window.hpp" "components/window.cpp"
    "components/box2D.hpp" "components/box2D.cpp"
    "components/ui.hpp" "components/ui.cpp"
    "components/sprite.hpp" "components/sprite.cpp"
    "components/tilebody.hpp" "components/tilebody.cpp"
    "components/camera.hpp" "components/camera.cpp")

target_sources(spock_engine PRIVATE
    "utility/ui.hpp" "utility/ui.cpp"
    "utility/entity.hpp" "utility/entity.cpp")

target_sources(spock_engine PRIVATE
    "data/text.hpp" "data/text.cpp"
    "data/resource_manager.hpp" "data/resource_manager.cpp"
    "data/atlas.hpp" "data/atlas.cpp"
    "data/tiles.hpp" "data/tiles.cpp"
    "data/tilemap.hpp" "data/tilemap.cpp")    

target_sources(spock_engine PUBLIC
    "spock.hpp")