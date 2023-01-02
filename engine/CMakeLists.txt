
add_library(spock_engine STATIC "spock.hpp")

target_compile_definitions(flecs_static PUBLIC ecs_ftime_t=double)

glad_add_library(glad_gl4_4 STATIC API gl:core=4.4)

target_link_libraries(spock_engine PUBLIC
    SDL3 glad_gl4_4 flecs_static glm box2d freetype)

target_include_directories(spock_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "spock.cpp"
    "base.hpp"
    "pipeline.hpp"
    "systems.hpp" "systems.cpp"
    "state.hpp" "state.cpp"
    "events.hpp" "events.cpp"
    "debug.hpp" "debug.cpp"
    "logger.hpp" "logger.cpp")

    
target_sources(spock_engine PRIVATE
    "components/window.hpp" "components/window.cpp"
    "components/box2D.hpp" "components/box2D.cpp"
    "components/ui.hpp" "components/ui.cpp"
    "components/sprite.hpp" "components/sprite.cpp"
    "components/tilemap.hpp" "components/tilemap.cpp"
    "components/camera.hpp" "components/camera.cpp"
    "components/particles.hpp" "components/particles.cpp")
    
target_sources(spock_engine PRIVATE
    "systems/window.hpp" "systems/window.cpp"
    "systems/render.hpp" "systems/render.cpp"
    "systems/physics.hpp" "systems/physics.cpp"
    "systems/primitive.hpp" "systems/primitive.cpp"
    "systems/sprite.hpp" "systems/sprite.cpp"
    "systems/ps_tracker.hpp" "systems/ps_tracker.cpp"
    "systems/ui.hpp" "systems/ui.cpp"
    "systems/camera.hpp" "systems/camera.cpp"
    "systems/particles.hpp" "systems/particles.cpp"
    "systems/tilemap.hpp" "systems/tilemap.cpp")

target_sources(spock_engine PRIVATE
    "utility/ui.hpp" "utility/ui.cpp"
    "utility/entity.hpp" "utility/entity.cpp"
    "utility/structure.hpp" "utility/structure.cpp"
    "utility/memory.hpp" "utility/memory.cpp")

target_sources(spock_engine PRIVATE
    "data/text.hpp" "data/text.cpp"
    "data/resource_manager.hpp" "data/resource_manager.cpp"
    "data/atlas.hpp" "data/atlas.cpp"
    "data/tiles.hpp" "data/tiles.cpp")

target_sources(spock_engine PRIVATE
    "render/opengl.hpp" "render/opengl.cpp"
    "render/render_system.hpp" "render/render_system.cpp"
    "render/meshes.hpp" "render/meshes.cpp"
    "render/base_renderer.hpp" "render/base_renderer.cpp"
    "render/button_renderer.hpp" "render/button_renderer.cpp"
    "render/font_renderer.hpp" "render/font_renderer.cpp"
    "render/sprite_renderer.hpp" "render/sprite_renderer.cpp"
    "render/primitive_renderer.hpp" "render/primitive_renderer.cpp"
    "render/ui_fb_renderer.hpp" "render/ui_fb_renderer.cpp")

target_sources(spock_engine PRIVATE
    "external/stb_image.h" "external/stb_image.c")