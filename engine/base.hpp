#pragma once 

#include "framework.hpp"
#include "debug.hpp"

#define SPOCK_ENGINE 1


#define spk_register_component(world, type)\
        world.component<type>() \
            .on_add([](type& comp) { \
                SPK_DEBUG_EXPR_IF(sfk::log.log("on add hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                comp.init(); \
            }) \
            .on_set([](type& comp) { \
                SPK_DEBUG_EXPR_IF(sfk::log.log("on set hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
            }) \
            .on_remove([](type& comp){  \
                SPK_DEBUG_EXPR_IF(sfk::log.log("on remove hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                comp.free(); \
            })

