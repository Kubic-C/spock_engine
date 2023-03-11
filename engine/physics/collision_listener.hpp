#pragma once

#include "base.hpp"

namespace spk {
    typedef std::function<void(flecs::entity a, flecs::entity other, b2Contact*)> begin_contact_t;
    typedef std::function<void(flecs::entity a, flecs::entity other, b2Contact*)> end_contact_t;

    struct comp_contact_callback_t {
        begin_contact_t begin = nullptr;
        end_contact_t   end   = nullptr;
    };

    class collision_listener_t : public b2ContactListener {
    public:
        void BeginContact(b2Contact* contact) override;
	    void EndContact(b2Contact* contact) override;
	    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
    };
}