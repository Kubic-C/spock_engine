#pragma once

#include "body.hpp"

#define SPK_MAX_QUAD_TREE_DEPTH 6

#define spk_get_quad_tree_loc(body_) \
    ((quad_tree_location_t*)(((uint8_t*)body_ - offsetof(quad_tree_body_t, body))  + offsetof(quad_tree_body_t, loc)))

namespace spk {
    struct quad_tree_t;
    struct world_tree_t;

    typedef std::list<const rigid_body_t*, memory_pool_t<const rigid_body_t*>> body_list_t;

    enum quad_tree_type_e: uint8_t {
        QUAD_TREE_TYPE_ROOT = 10
    };

    struct quad_tree_location_t {
        quad_tree_t* tree;
        std::list<const rigid_body_t*>::iterator iter;
    };

    struct quad_tree_body_t {
        rigid_body_t body;
        quad_tree_location_t loc;
    };

    struct quad_tree_t {
        quad_tree_t();
        ~quad_tree_t();

        bool can_fit(const aabb_pos_t& pos);
        bool can_fit_branch(const aabb_pos_t& pos);
        bool insert(object_pool_t<quad_tree_t>& pool, const rigid_body_t* body);
        void remove(const rigid_body_t* body);
        bool relocate(const rigid_body_t* body);
        glm::vec2 half_size_of_half_size() const;

        inline bool insert_to_branch(object_pool_t<quad_tree_t>& pool, const aabb_pos_t& area, uint8_t index, const rigid_body_t* body) {
            spk_trace(); 

            if(branches[index] == nullptr) {
                branches[index] = pool.create(1);

                spk_assert(branches[index] != nullptr, "could not allocate branch");

                branches[index]->area   = area;
                branches[index]->type   = (quad_tree_type_e)index;
                branches[index]->parent = this;
                branches[index]->index  = UINT32_MAX;
                branches[index]->depth  = depth + 1;
                branches[index]->root   = root;
            }

            return branches[index]->insert(pool, body);
        }

        aabb_pos_t bl() const;
        aabb_pos_t br() const;
        aabb_pos_t tr() const;
        aabb_pos_t tl() const;

        void add_size(int amount) {
            culm_size += amount;

            if(parent) {
                parent->add_size(amount);
            }
        }

        void iterate(std::function<void(quad_tree_t* tree)> clbk) {
            clbk(this);

            for(auto branch : branches) {
                if(branch)
                    branch->iterate(clbk);
            }
        }

        // if it returns true it means the node itself was destroyed.
        // this used when iterating over quad trees to remove empty ones to prevent
        // accidently reading data from derefernced wild pointers
        bool destroy_empty();

        size_t             culm_size;
        size_t             depth;
        int32_t            index;
        quad_tree_type_e   type;
        world_tree_t*      root;
        quad_tree_t*       parent;
        aabb_pos_t         area;
        body_list_t        contained_bodies;
        std::array<quad_tree_t*, 4>    branches;
    };

    class world_tree_t {
    public:
        world_tree_t(glm::vec2 area) 
            : quad_tree_pool(1000) {
            this->area.aabb = area;
        }

        bool insert(const rigid_body_t* body);
        void iterate(std::function<void(quad_tree_t*)>&& clbk);
        void remove(const rigid_body_t* body);
        bool relocate(const rigid_body_t* body);

        body_list_t                        global_contained_bodies;
        aabb_pos_t                         area;
        object_pool_t<quad_tree_t>         quad_tree_pool;
        hashmap_t<float, quad_tree_t*>     trees;
    };
}