/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "physics.hpp"
#include "core/internal.hpp"

namespace spk {
    class physics_system_t {
    public:
        physics_system_t() {
            world = new kin::world_t(glm::vec2(0.0f, -9.81f));
        }

        ~physics_system_t() {
            // when b2World is deleted all b2Bodies are invalid,
            // this of course includes ptrs pointing to them, we must 
            // set all rigid body components to nullptr as they have ptrs
            // and upon deletion of a rigid body component, if their ptr is set to something other
            // then nullptr they will try to delete them in the deconstructor
            ecs_world().each([&](spk::comp_rigid_body_t& rb){
                rb = nullptr;
            });

            delete world;
        }

        kin::world_t* world;
    };

    void physics_system_tick(flecs::iter iter) {
        auto physics_world = internal->scene.physics_world;
    
        physics_world->update(iter.delta_time(), 6);
    }

    void physics_cs_init(const flecs::world& world) {
        spk_trace();        

        rigid_body_comp_init(world);

        auto physics_system = scene().stack.push<physics_system_t>();        
        
        internal->scene.physics_world = physics_system->world;

        world.system().iter(physics_system_tick);

    }
}