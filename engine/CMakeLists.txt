
add_library(spock_engine STATIC "spock.hpp")

target_compile_definitions(flecs_static PUBLIC ecs_ftime_t=double)

glad_add_library(glad_gl4_4 STATIC API gl:core=4.4)

target_link_libraries(spock_engine PUBLIC 
    SDL2-static SDL2_mixer glad_gl4_4 flecs_static glm freetype ${SPK_DEBUG_LIBS})

target_include_directories(spock_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")

add_subdirectory("spox2d")

target_sources(spock_engine PRIVATE
    "base.hpp"
    "spock.cpp"
    "pipeline.hpp"
    "debug.hpp" "debug.cpp"
    "logger.hpp" "logger.cpp")

target_sources(spock_engine PRIVATE
    "components/components.hpp" "components/components.cpp"
    "components/sprite.hpp" "components/sprite.cpp"
    "components/tilemap.hpp" "components/tilemap.cpp"
    "components/camera.hpp" "components/camera.cpp"
    "components/particles.hpp" "components/particles.cpp"
    "components/character_controller.hpp" "components/character_controller.cpp"
    "components/rigid_body.hpp" "components/rigid_body.cpp")
    
target_sources(spock_engine PRIVATE
    "systems/components.hpp" "systems/components.cpp"
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
    "utility/stack_allocator.hpp" "utility/stack_allocator.cpp"
    "utility/ui.hpp" "utility/ui.cpp"
    "utility/entity.hpp" "utility/entity.cpp"
    "utility/structures.hpp" "utility/structures.cpp"
    "utility/static_list.hpp" "utility/static_list.cpp"
    "utility/array2D.hpp" "utility/array2D.cpp"
    "utility/memory_pool.hpp" "utility/memory_pool.cpp"
    "utility/hashmap.hpp" "utility/hashmap.cpp"
    "utility/image_loader.hpp" "utility/image_loader.cpp"
    "utility/allocator.hpp" "utility/allocator.cpp")

target_sources(spock_engine PRIVATE)

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
    "core/sound.hpp" "core/sound.cpp"
    "core/data.hpp" "core/data.cpp"
    "core/internal.hpp" "core/internal.cpp"
    "core/events.hpp" "core/events.cpp"
    "core/window.hpp" "core/window.cpp"
    "core/ui.hpp" "core/ui.cpp"    
    "core/text.hpp" "core/text.cpp"
    "core/sprite_atlas.hpp" "core/sprite_atlas.cpp"
    "core/sprite_array.hpp" "core/sprite_array.cpp"
    "core/tiles.hpp" "core/tiles.cpp")

target_sources(spock_engine PRIVATE
    "physics/collision_listener.hpp" "physics/collision_listener.cpp")

target_sources(spock_engine PRIVATE
    "external/stb_image.h" "external/stb_image.c")