
add_library(spock_engine STATIC "spock.hpp")

target_compile_definitions(flecs_static PUBLIC ecs_ftime_t=double)

glad_add_library(glad_gl4_4 STATIC API gl:core=4.4)

target_link_libraries(spock_engine PUBLIC 
    SDL3 glad_gl4_4 flecs_static glm freetype)

target_include_directories(spock_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "base.hpp"
    "spock.cpp"
    "pipeline.hpp"
    "systems.hpp" "systems.cpp"
    "state.hpp" "state.cpp"
    "events.hpp" "events.cpp"
    "debug.hpp" "debug.cpp"
    "logger.hpp" "logger.cpp")

    
target_sources(spock_engine PRIVATE
    "components/window.hpp" "components/window.cpp"
    "components/ui.hpp" "components/ui.cpp"
    "components/sprite.hpp" "components/sprite.cpp"
    "components/tilemap.hpp" "components/tilemap.cpp"
    "components/camera.hpp" "components/camera.cpp"
    "components/particles.hpp" "components/particles.cpp"
    "components/character_controller.hpp" "components/character_controller.cpp")
    
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
    "systems/tilemap.hpp" "systems/tilemap.cpp"
    "systems/character_controller.hpp" "systems/character_controller.cpp")

target_sources(spock_engine PRIVATE
    "utility/ui.hpp" "utility/ui.cpp"
    "utility/entity.hpp" "utility/entity.cpp"
    "utility/structures.hpp" "utility/structures.cpp"
    "utility/static_list.hpp" "utility/static_list.cpp"
    "utility/array2D.hpp" "utility/array2D.cpp"
    "utility/memory_pool.hpp" "utility/memory_pool.cpp"
    "utility/hashmap.hpp" "utility/hashmap.cpp"
    "utility/image_loader.hpp" "utility/image_loader.cpp")

target_sources(spock_engine PRIVATE
    "data/text.hpp" "data/text.cpp"
    "data/resource_manager.hpp" "data/resource_manager.cpp"
    "data/sprite_atlas.hpp" "data/sprite_atlas.cpp"
    "data/sprite_array.hpp" "data/sprite_array.cpp"
    "data/tiles.hpp" "data/tiles.cpp")

target_sources(spock_engine PRIVATE
    "render/opengl.hpp" "render/opengl.cpp"
    "render/texture_array.hpp" "render/texture_array.cpp"
    "render/render_system.hpp" "render/render_system.cpp"
    "render/meshes.hpp" "render/meshes.cpp"
    "render/base_renderer.hpp" "render/base_renderer.cpp"
    "render/button_renderer.hpp" "render/button_renderer.cpp"
    "render/font_renderer.hpp" "render/font_renderer.cpp"
    "render/sprite_renderer.hpp" "render/sprite_renderer.cpp"
    "render/primitive_renderer.hpp" "render/primitive_renderer.cpp"
    "render/ui_fb_renderer.hpp" "render/ui_fb_renderer.cpp")

target_sources(spock_engine PRIVATE
    "physics/transform.hpp" "physics/transform.cpp"
    "physics/material.hpp" "physics/material.cpp"
    "physics/body.hpp" "physics/body.cpp"
    "physics/aabb.hpp" "physics/aabb.cpp"
    "physics/world.hpp" "physics/world.cpp")

target_sources(spock_engine PRIVATE
    "external/stb_image.h" "external/stb_image.c")