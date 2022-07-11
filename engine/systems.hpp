#pragma once
#include "scene.hpp"

#define USER_SORTING_ORDER0 0U
#define USER_SORTING_ORDER1 1U
#define USER_SORTING_ORDER2 2U
#define USER_SORTING_ORDER3 3U
#define USER_SORTING_ORDER4 4U
#define DEFAULT_SORTING_ORDER 5U

namespace spk {
    struct transform_tt {
        bool freeze;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    struct message_tt {
        uint32_t code; // message code
        std::string str; // may be empty

        message_tt(const uint32_t& code, const std::string& str) 
            : code(code), str(str) {}
    };

    struct system_tt {
        uint32_t sorting_order = USER_SORTING_ORDER0;
        std::string name;
        std::queue<message_tt>* bus;

        // called by system manager (part of game engine)
        virtual void handle_message(scene_tt& scene, float deltatime, message_tt& message) {}
        virtual void init(scene_tt& scene, void* data) {}
        virtual void update(scene_tt& scene, float deltatime) {}
        virtual void tick(scene_tt& scene, float deltatime) {}
        virtual void free() {}
    };

    /* handles updating and keeping track of the active systems*/
    struct system_manager_tt {
        std::vector<system_tt*> systems;
        std::queue<message_tt> messages;

        void push_system(system_tt* system);

        void msg_update(scene_tt& scene, float deltatime);
        void update(scene_tt& scene, float deltatime); 
        void tick(scene_tt& scene, float deltatime);
        void free();
        void free_user_systems();
    };

    struct render_system_tt {
        virtual void init(sfk::window_tt& window, flecs::world& world, void* scene) {}
        virtual void render(scene_tt& scene) {}
        virtual void resize(int width, int height) {}
        virtual void free() {}
    };

    struct render_system_manager_tt {
        std::vector<render_system_tt*> systems;

        void push_system(render_system_tt* system);
        void free();
    };
}

constexpr uint32_t _SPK_CHAR(const char* key) {
    return (uint32_t)key[0];
}

#define SPK_ENUM_KEY(key__) key_ ## key__ = _SPK_CHAR(#key__)

namespace spk { 
    enum message_code: uint32_t {
        // lowercase
        SPK_ENUM_KEY(q),
        SPK_ENUM_KEY(w),
        SPK_ENUM_KEY(e),
        SPK_ENUM_KEY(r),
        SPK_ENUM_KEY(t),
        SPK_ENUM_KEY(y),
        SPK_ENUM_KEY(u),
        SPK_ENUM_KEY(i),
        SPK_ENUM_KEY(o),
        SPK_ENUM_KEY(p),
        SPK_ENUM_KEY(a),
        SPK_ENUM_KEY(s),
        SPK_ENUM_KEY(d),
        SPK_ENUM_KEY(f),
        SPK_ENUM_KEY(g),
        SPK_ENUM_KEY(h),
        SPK_ENUM_KEY(j),
        SPK_ENUM_KEY(k),
        SPK_ENUM_KEY(l),
        SPK_ENUM_KEY(z),
        SPK_ENUM_KEY(x),
        SPK_ENUM_KEY(c),
        SPK_ENUM_KEY(v),
        SPK_ENUM_KEY(b),
        SPK_ENUM_KEY(n),
        SPK_ENUM_KEY(m),
        // UPPERCASE
        SPK_ENUM_KEY(Q),
        SPK_ENUM_KEY(W),
        SPK_ENUM_KEY(E),
        SPK_ENUM_KEY(R),
        SPK_ENUM_KEY(T),
        SPK_ENUM_KEY(Y),
        SPK_ENUM_KEY(U),
        SPK_ENUM_KEY(I),
        SPK_ENUM_KEY(O),
        SPK_ENUM_KEY(P),
        SPK_ENUM_KEY(A),
        SPK_ENUM_KEY(S),
        SPK_ENUM_KEY(D),
        SPK_ENUM_KEY(F),
        SPK_ENUM_KEY(G),
        SPK_ENUM_KEY(H),
        SPK_ENUM_KEY(J),
        SPK_ENUM_KEY(K),
        SPK_ENUM_KEY(L),
        SPK_ENUM_KEY(Z),
        SPK_ENUM_KEY(X),
        SPK_ENUM_KEY(C),
        SPK_ENUM_KEY(V),
        SPK_ENUM_KEY(B),
        SPK_ENUM_KEY(N),
        SPK_ENUM_KEY(M),
        ascii_end = 255U
    };    
}