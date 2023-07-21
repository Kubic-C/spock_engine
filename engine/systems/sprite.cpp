/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite.hpp"
#include "core/internal.hpp"

namespace spk {
    void animate_system(flecs::iter& iter, comp_sprite_t* sprites, comp_animate_t* animations) {

        for(auto i : iter) {
            comp_sprite_t&  sprite = sprites[i];
            comp_animate_t& animation = animations[i];

            if(animation.last_switch + animation.switch_time < spk::time.get_time() &&
               animation.on) {
                if(sprite.index == animation.index_end) {
                    sprite.index = animation.index_begin;
                } else {
                    sprite.index++;
                }

                animation.last_switch = spk::time.get_time();
            }
        }
    }

    void sprite_cs_init(const flecs::world& world) {
        sprite_comp_init(world);

        world.system<comp_sprite_t, comp_animate_t>().iter(animate_system);
    }
}