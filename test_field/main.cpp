#include "spock.hpp"
#include "utility/image_loader.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

void create_usless_box() {
    auto box = spk::ecs_world().entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_sprite_t& sprite) {
        float random = rand();
        random *= 0.01f;

        rb->set_position({50.0f, 100.0f});

        const float h = 1.0f;
        const float w = 1.0f;
        const float u = 1.0f;
        uint32_t s = 0;
        for(uint32_t i = 0; i < 1; i++) {
            kin::rigid_body_t* body = rb;

            glm::vec2 pos = { -w, -h };
            while(pos.y < h) {
                s++;
                kin::fixture_def_t def;
                def.density = 1.0f;
                def.hw = u;
                def.hh = u;
                def.restitution = 1.0f;
                def.rel_pos = pos;
                body->create_fixture(def);

                pos.x += u * 2.0f;
                if(pos.x >= w) {
                    pos.x = -w;
                    pos.y += u * 2.0f;
                }
            }
        }

        sprite.size = {1.0f, 1.0f};
        sprite.id = 1;
        sprite.index = 1;
        sprite.z = 1.0f;
    });

    if(rand() % 10 < 3) {
        box.set([](spk::comp_sprite_t& sprite, spk::comp_animate_t& animate) {
            sprite.index = 3;
            animate.index_begin = 3;
            animate.index_end   = 5;
            animate.on = true;
            animate.switch_time = 0.5f;
        });
    }
}

int main(int argc, char* argv[]) {
    spk_trace();

    spk::log.log("running..");
    int code = 0;

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    spk::init();

    spk::print_deps_versions();

    spk::settings_t&   settings  = spk::settings();
    spk::resources_t&  resources = spk::resources();
    spk::scene_t&      scene     = spk::scene();
    spk::canvas_t&     canvas    = spk::canvas();
    kin::world_t*      world     = scene.physics_world;
    uint32_t           smells_blood_id, coin_sound_id;
    uint32_t           sprite_array_id, high_def_array;
    std::array<uint32_t, 5> fonts;
    ptm::ptr_t<spk::text_t> text;

    scene.window->size_set(1000, 600);

    {
        smells_blood_id = spk::music_create("smells_blood.mp3");
        coin_sound_id   = spk::chunk_create("coin_sound.wav");
        spk::music_play(smells_blood_id, 1);

        spk::audio_chunk_volume(1);
        spk::audio_music_volume(1);
        world->set_gravity({0.0f, -10.0f});

        fonts[0] = spk::font_create("./fonts/DotGothic16-Regular.ttf", 128, 20);
        fonts[1] = spk::font_create("./fonts/eater.regular.ttf", 128, 20);
        fonts[2] = spk::font_create("./fonts/FiraCode-Bold.ttf", 128, 20);
        fonts[3] = spk::font_create("./fonts/GajrajOne-Regular.ttf", 128, 20);
        fonts[4] = spk::font_create("./fonts/PressStart2P.ttf", 128, 20);

        sprite_array_id = spk::sprite_array_create(32, 32, 6);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image1.png", 0);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image2.png", 1);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image3.png", 2);
        spk::sprite_array_set(sprite_array_id, "./texture_array/anim1.png", 3);
        spk::sprite_array_set(sprite_array_id, "./texture_array/anim2.png", 4);
        spk::sprite_array_set(sprite_array_id, "./texture_array/anim3.png", 5);

        high_def_array = spk::sprite_array_create(1000, 1000, 2);
        spk::sprite_array_set(high_def_array, "./texture_array/windows_logo.png", 0);
    }

    { // canvas 
        canvas.font = fonts[2];

        text = canvas.element<spk::text_t>();
        text->x_set(spk::constraint_relative(0.05f))
             .y_set(spk::constraint_relative(0.85f))
             .width_set(spk::constraint_relative(0.5f))
             .height_set(spk::constraint_relative(0.1f));
        text->text       = "loading..";
        text->text_color = {1.0f, 1.0f, 1.0f};
        text->color      = {1.0f, 1.0f, 1.0f, 0.0f};

        ptm::ptr_t button = text->element<spk::button_t>();
        button->x_set(spk::constraint_center())
               .y_set(spk::constraint_relative(0.0f))
               .width_set(spk::constraint_relative(0.3f))
               .height_set(spk::constraint_relative(0.2f));
        button->color       = {1.0f, 0.0f, 0.0f, 1.0f};        
        button->hover_color = {1.0f, 0.4f, 0.4f};
        button->click_color = {1.0f, 0.5f, 0.5f};    
        button->callback = [&](spk::button_t& button) {
            spk::exit();
        };

        ptm::ptr_t button_text = button->element<spk::text_t>();
        button_text->x_set(spk::constraint_center())
                    .y_set(spk::constraint_relative(0.0f))
                    .width_set(spk::constraint_relative(1.0f))
                    .height_set(spk::constraint_relative(1.0f));
        button_text->color = {0.0f, 0.0f, 0.0f, 0.0f};
        button_text->text  = "click button to exit";

        spk::settings().target_tps = 60;
        spk::settings().target_fps = 75;
    }

    for(size_t i = 0; i < 10; i++) {
        create_usless_box();
    }

    spk::tile_t stone;
    stone.density = 1.0f;
    stone.friction = 10.0f;
    stone.flags = spk::TILE_FLAGS_COLLIADABLE;
    stone.friction = 1.0f;
    stone.restitution = 0.0f;
    stone.sprite.id    = sprite_array_id;
    stone.sprite.index = 1;

    spk::tile_t bg_wood = stone;
    bg_wood.sprite.id    = sprite_array_id;
    bg_wood.sprite.z     = -5.0f;
    bg_wood.flags        = 0;
    bg_wood.sprite.index = 0;

    spk::tile_t p_block = stone;
    p_block.sprite.id    = sprite_array_id;
    p_block.sprite.index = 3;
    p_block.animation.on = true;
    p_block.animation.index_begin = 3;
    p_block.animation.index_end = 5;
    p_block.animation.switch_time = 1.0f;
    p_block.density = 2.0f;

    flecs::entity wall = scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tm){
        rb->type = kin::body_type_static;
        rb->set_position({0.0f, 0.0f});

        const int width  = 100;
        const int height = 100;

        for(uint32_t x = 0; x < width; x++) {
            for(uint32_t y = 0; y < height; y++) {
                if(x != 0 && x != (width - 1) &&
                    y != 0 && y != (height - 1)) {
                    tm.get (x, y) = bg_wood;
                } else {
                    tm.get(x, y) = stone;
                }
            }
        }
    });

    flecs::entity character;

    for(size_t i = 0; i < 1; i++)
        character = spk::ecs_world().entity().set([&](
                spk::comp_rigid_body_t& rb, 
                spk::comp_character_controller_t& cc,
                spk::comp_tilemap_t& tilemap,
                spk::comp_camera_t& camera) {
            rb->set_position({5.0f + i * 5.0f, 50.0f});
            rb->apply_linear_velocity({0.0f, -40.0f});

            tilemap.get(2, 0) = stone;
            tilemap.get(2, 1) = stone;
            tilemap.get(2, 2) = stone;
            tilemap.get(2, 3) = stone;
            tilemap.get(2, 4) = stone;
            tilemap.get(2, 5) = p_block;

            cc.speed = 10;
        });

    character.add<spk::tag_current_camera_t>();

    spk::ecs_world().observer().event<spk::event_mouse_wheel_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_mouse_wheel_t>();
            auto camera = scene.camera.get_ref<spk::comp_camera_t>();
            float scale = 0.1f;

            if(event->y < 0) {
                camera->scale -= scale;
            } else if(event->y > 0) {
                camera->scale += scale;
            }
        });

    scene.user_data.update = [&](){
        if(spk::window().key_get(SDL_SCANCODE_2)) {
            canvas.font = fonts[0];
        }
        else if(spk::window().key_get(SDL_SCANCODE_3)) {
            canvas.font = fonts[1];
        }
        else if(spk::window().key_get(SDL_SCANCODE_4)) {
            canvas.font = fonts[2];
        }
        else if(spk::window().key_get(SDL_SCANCODE_5)) {
            canvas.font = fonts[3];
        }
        else if(spk::window().key_get(SDL_SCANCODE_6)) {
            canvas.font = fonts[4];
        }

        text->text = std::string(spk::build_name()) + " | FPS:" + std::to_string(spk::statistics().fps) + " | TPS:"  + std::to_string(spk::statistics().tps);

        character.set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tilemap, spk::comp_camera_t& camera){
            camera.pos = rb->get_world_pos();

            text->text += "\nvelocity.xy:" + std::to_string(rb->linear_vel.x) + " / " + std::to_string(rb->linear_vel.y);
        
            if(spk::window().key_get(SDL_SCANCODE_1)) {
                rb->linear_vel = {0.0f, 0.0f};
            }
        });
    };  

    code = spk::run();

    spk::free();

    return code;
}