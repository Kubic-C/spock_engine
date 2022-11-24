#pragma once

#include <framework.hpp>

/* handling of systems with FLECS */

#define SPK_GET_CTX_REF(iter, type) static_cast<flecs::entity*>(iter.ctx())->get_ref<type>()

namespace spk {
    class system_ctx_allocater_t {
    public:
        void init(flecs::world& world);
        void free();

        template<typename T>
        flecs::entity* allocate_ctx() {
            flecs::entity* e = ctx_name_list.malloc();

            *e =  world->entity().add<T>();

            return e;
        }

    private:
        flecs::world* world;
        sfk::memory_pool_t<flecs::entity, 100, 4> ctx_name_list;
    };

}