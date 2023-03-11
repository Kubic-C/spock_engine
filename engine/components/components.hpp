#pragma once

#include "base.hpp"

#define spk_register_component(world, type) \
        world.component<type>() \
            .on_add([](flecs::entity _entity, type& _comp) { \
                spk_trace(); \
                SPK_DEBUG_EXPR_IF(log.log("on add hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                _comp.init(_entity); \
            }) \
            .on_remove([](flecs::entity _entity, type& _comp){ \
                spk_trace(); \
                SPK_DEBUG_EXPR_IF(log.log("on remove hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                _comp.free(_entity); \
            }) 

namespace spk {
    struct tag_should_remove_t {};

    struct component_t {
        virtual void init(flecs::entity entity) {}
        virtual void free(flecs::entity entity) {}
    };
}