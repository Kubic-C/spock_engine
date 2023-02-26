#include "world.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    rigid_body_t* physics_world_t::add(glm::vec2 pos) {
        auto body = body_list.insert(body_list.cend(), quad_tree_body_t());
    
        body->body.transform.pos    = pos;
        body->body.max              = aabb_pos_t(glm::vec2(0.0f, 0.0f), aabb_t(0.0f, 0.0f));
        body->body.angular_velocity = 0;
        body->body.fixtures         = nullptr;
        body->body.fixtures_size    = 0;

        if(!tree.insert(&body->body)) {
            body_list.erase(body);
            return nullptr;
        }

        return &body->body;
    }

    fixture_t* physics_world_t::add_fixture(rigid_body_t* rigid_body, const material_t& material, glm::vec2 pos) {
        auto fixture      = fixture_pool.create(1);
        fixture->rb       = rigid_body;
        fixture->pos      = pos;
        fixture->material = material;

        if(rigid_body->fixtures_size == 0) {
            rigid_body->fixtures = fixture;
        } else {
            fixture_t* last = nullptr;

            for(fixture_t* current = rigid_body->fixtures; current != nullptr;) {
                last = current;
                
                if(current->next != nullptr) {
                    current = current->next;
                } else {
                    break;
                }
            }

            last->next = fixture;
            fixture->prev = last;
        }

        rigid_body->fixtures_size++;
    
        return fixture;
    }

    void physics_world_t::add_box_fixture(rigid_body_t* rigid_body, const material_t& material, glm::vec2 pos, float hw, float hh) {
        auto fixture = add_fixture(rigid_body, material, pos);

        fixture->type = FIXTURE_TYPE_BOX;
        fixture->def.box.hw = hw;
        fixture->def.box.hh = hh;
    }

    void physics_world_t::step(float delta_time) {
        spk_trace();

        for(auto& body : body_list) {
            body.body.velocity += gravity;
        
            body.body.transform.pos += body.body.velocity * delta_time;

            body.body.transform.calculate();
            body.body.calculate_max();

            if(body.loc.tree == nullptr) {
                // if it is in global space, we want to remove it as fast as possible
                // it can potentially slow down the program
                
                tree.relocate(&body.body);
                continue;
            }

            if(!body.loc.tree->can_fit(body.body.get_aabb_pos()) ||
                body.loc.tree->can_fit_branch(body.body.get_aabb_pos())) {
                
                if(!body.loc.tree->relocate(&body.body)) {
                    spk_assert(!"could not fit tree");
                }
            }
        }

        // safely iterate over a map that's size is being changed within the for loop
        for(auto iter = tree.trees.begin(); iter != tree.trees.end(); iter++) {         
            if(iter->second->destroy_empty()) {
                iter = tree.trees.begin();

                if(iter == tree.trees.end())
                    break;
            }
        }
    }
}