#include "physics.hpp"

namespace spk {
    physics2D_tt::physics2D_tt() {

    }

    void physics2D_tt::init(scene_tt& scene, void* data) {
        scene.world.component<body2D_tt>();
        scene.world.component<aabb_tt>();

    }
    
    void physics2D_tt::tick(scene_tt& scene, float deltatime) {
        flecs::world& world = scene.world;

        { // rigidbody applications
            auto q = world.query<transform_tt, body2D_tt>();

            q.iter([&](flecs::iter& it, transform_tt* c_transforms, body2D_tt* c_rigidbody2D) {
                for(auto i : it) {
                    transform_tt& transform = c_transforms[i];
                    body2D_tt& rigidbody2D = c_rigidbody2D[i];

                    transform.position.y += -9.81f * deltatime;
                    transform.position.y -= (rigidbody2D.gravity * deltatime) * !rigidbody2D.kinematic; // magic gravity 
                    transform.position += glm::vec3(rigidbody2D.velocity * deltatime, 0.0f);
                }
            });
        }

        { // check if our object is colliding with every other object, for every object
            auto q = world.query<transform_tt, aabb_tt, body2D_tt>();
            auto q2 = world.query<transform_tt, aabb_tt, body2D_tt>();

            // flecs locks tables during iteration, so well have to call the callbacks
            // after iterating, to prevent an internal or table locked error within flecs
            //TODO: list is allocated on heap in a time sensitive function, optimize
            std::vector<std::pair<flecs::entity, std::pair<collision_callback_tt, collision_info_tt>>> callback_list; 

            q.iter([&](flecs::iter& it, transform_tt* c_transforms, aabb_tt* c_aabb, body2D_tt* c_body1) {
                rect_tt temp_rect[2];

                for(auto i : it) {
                    transform_tt* transform1 = &c_transforms[i];
                    aabb_tt* aabb1           = &c_aabb[i];
                    body2D_tt* body1         = &c_body1[i];

                    q2.iter([&](flecs::iter& it2, transform_tt* c_transforms2, aabb_tt* c_aabb2, body2D_tt* c_body2) {
                        for(auto j : it2) {
                            transform_tt* transform2 = &c_transforms2[j];
                            aabb_tt* aabb2           = &c_aabb2[j];
                            body2D_tt* body2         = &c_body2[j];
                            collision_info_tt ci;

                            ci.one.aabb = aabb1;
                            ci.one.body = body1;
                            ci.one.transform = transform1;

                            ci.two.aabb = aabb2;
                            ci.two.body = body2;
                            ci.two.transform = transform2;

                            if(it.entity(i).id() == it2.entity(j).id()) {
                                continue;
                            }

                            if(aabb_vs_aabb(&ci, temp_rect)) {
                                collision_resolution(&ci, temp_rect, deltatime);
                                
                                if(body1->collision_callback)
                                    callback_list.push_back(std::pair(it.entity(i), std::pair(body1->collision_callback, ci)));

                                ci.cn = -ci.cn;

                                if(body2->collision_callback)
                                    callback_list.push_back(std::pair(it2.entity(j), std::pair(body2->collision_callback, ci)));

                                continue;
                            } 
                        }
                    });
                }
            });

            for(auto pair : callback_list) {
                // not confusing to look at at all
                // callback vv    // entity vv   // collision info
                //pair.second.first(pair.first, pair.second.second);
            }
        }
    }

    void physics2D_tt::free() {

    }
}