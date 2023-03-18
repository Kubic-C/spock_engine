/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "collision_listener.hpp"

namespace spk {
    void collision_listener_t::BeginContact(b2Contact* contact) {
        flecs::entity entity_A = contact->GetFixtureA()->GetBody()->GetUserData().entity;
        flecs::entity entity_B = contact->GetFixtureB()->GetBody()->GetUserData().entity;

        if(entity_A.has<comp_contact_callback_t>()) {
            entity_A.set([&](comp_contact_callback_t& contact_callback){
                if(contact_callback.begin) {
                    contact_callback.begin(entity_A, entity_B, contact);
                }
            });
        }

        
        if(entity_B.has<comp_contact_callback_t>()) {
            entity_B.set([&](comp_contact_callback_t& contact_callback){
                if(contact_callback.begin) {
                    contact_callback.begin(entity_B, entity_A, contact);
                }
            });
        }
    }

    void collision_listener_t::EndContact(b2Contact* contact) {
        flecs::entity entity_A = contact->GetFixtureA()->GetBody()->GetUserData().entity;
        flecs::entity entity_B = contact->GetFixtureB()->GetBody()->GetUserData().entity;

        if(entity_A.has<comp_contact_callback_t>()) {
            entity_A.set([&](comp_contact_callback_t& contact_callback){
                if(contact_callback.end) {
                    contact_callback.end(entity_A, entity_B, contact);
                }
            });
        }

        
        if(entity_B.has<comp_contact_callback_t>()) {
            entity_B.set([&](comp_contact_callback_t& contact_callback){
                if(contact_callback.end) {
                    contact_callback.end(entity_B, entity_A, contact);
                }
            });
        }
    }

    void collision_listener_t::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

    }

    void collision_listener_t::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
        
    }
}