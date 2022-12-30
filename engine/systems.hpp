#pragma once

#include "base.hpp"
#include "utility/structure.hpp"

/* handling of systems with FLECS */

#define SPK_GET_CTX_REF(iter, type) static_cast<type*>(iter.ctx())
#define SPK_NOT_A_TAG  uint8_t ___not_a_tag;

namespace spk {
    class system_t {
    public:
        virtual void init() {}
        virtual void free() {}
    };

    class system_ctx_allocater_t {
    public:
        void init();
        void free();

        template<typename T>
        T* allocate_ctx() {
            T*& ctx = (T*&)ctx_name_list.emplace_back();

            // better allocation will come later i swear
            ctx = (T*)malloc(sizeof(T));

            new(ctx)T();

            static_cast<spk::system_t*>(ctx)->init();

            return ctx;
        }

    private:
        static_list<system_t*, 100> ctx_name_list;
    };

}