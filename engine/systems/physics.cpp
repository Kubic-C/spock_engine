#include "physics.hpp"

namespace spk {
    physics2D_tt::physics2D_tt() {

    }

    void physics2D_tt::init(scene_tt& scene, void* data) {
        scene.world.component<rigidbody2D_tt>();
        scene.world.component<aabb_tt>();

    }
    
    void physics2D_tt::tick(scene_tt& scene, float deltatime) {
        flecs::world& world = scene.world;

        { // rigidbody applications
            auto q = world.query<transform_tt, rigidbody2D_tt>();

            q.iter([&](flecs::iter& it, transform_tt* c_transforms, rigidbody2D_tt* c_rigidbody2D) {
                for(auto i : it) {
                    transform_tt& transform = c_transforms[i];
                    rigidbody2D_tt& rigidbody2D = c_rigidbody2D[i];

                    transform.position.y -= (rigidbody2D.gravity * deltatime) * !rigidbody2D.kinematic; // magic gravity 
                    transform.position += glm::vec3(rigidbody2D.velocity * deltatime, 0.0f);
                }
            });
        }

        { // check if our object is colliding with every other object, for every object
            auto q = world.query<transform_tt, aabb_tt>();
            auto q2 = world.query<transform_tt, aabb_tt>();

            // flecs locks tables during iteration, so well have to call the callbacks
            // after iterating, to prevent an internal or table locked error within flecs
            std::vector<std::pair<flecs::entity, std::pair<collision_callback_tt, collision_info_tt>>> callback_list; 

            q.iter([&](flecs::iter& it, transform_tt* c_transforms, aabb_tt* c_aabb) {
                rect_tt temp_rect[2];

                for(auto i : it) {
                    transform_tt& transform1 = c_transforms[i];
                    aabb_tt& aabb1 = c_aabb[i];

                    q2.iter([&](flecs::iter& it2, transform_tt* c_transforms2, aabb_tt* c_aabb2) {
                        for(auto j : it2) {
                            transform_tt& transform2 = c_transforms2[j];
                            aabb_tt& aabb2 = c_aabb2[j];
               
                            if(it.entity(i).id() == it2.entity(j).id()) {
                                continue;
                            }

                            if(aabb_vs_aabb(transform1, aabb1, transform2, aabb2, temp_rect)) {
                                collision_info_tt ci;

                                collision_resolution(transform1, transform2, temp_rect, ci);

                                {
                                    const rigidbody2D_tt *rb1;

                                    rb1 = it.entity(i).get<rigidbody2D_tt>();
                                    if(rb1)
                                        if(rb1->collision_callback)
                                            rb1->collision_callback(it.entity(i), ci);
                                }

                                ci.cn = -ci.cn;

                                {
                                    const rigidbody2D_tt* rb2;
                                    
                                    rb2 = it2.entity(j).get<rigidbody2D_tt>(); 
                                    if(rb2)
                                        if(rb2->collision_callback)
                                            callback_list.push_back(std::pair(it2.entity(j), std::pair(rb2->collision_callback, ci)));
                                }

                                continue;
                            } 
                        }
                    });
                }
            });

            for(auto pair : callback_list) {
                // not confusing to look at at all
                // callback vv    // entity vv   // collision info
                pair.second.first(pair.first, pair.second.second);
            }
        }
    }

    void physics2D_tt::free() {

    }
}