#pragma once

#include "body.hpp"

#define SPK_MAX_QUAD_TREE_DEPTH 10

#define spk_get_quad_tree_loc(body_) \
    ((quad_tree_location_t*)(((uint8_t*)body_ - offsetof(quad_tree_body_t, body))  + offsetof(quad_tree_body_t, loc)))

namespace spk {
    struct quad_tree_t;
    struct world_tree_t;

    enum quad_tree_type_e: uint8_t {
        QUAD_TREE_TYPE_ROOT = 10
    };

    struct quad_tree_location_t {
        quad_tree_t*         tree;
        const rigid_body_t** ptr;
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
        bool insert(memory_pool_t<quad_tree_t>& pool, const rigid_body_t* body);
        void remove(const rigid_body_t* body);
        bool relocate(const rigid_body_t* body);
        bool insert_to_branch(memory_pool_t<quad_tree_t>& pool, const aabb_pos_t& area, uint8_t index, const rigid_body_t* body);
        glm::vec2 half_size_of_half_size() const;

        aabb_pos_t bl() const;
        aabb_pos_t br() const;
        aabb_pos_t tr() const;
        aabb_pos_t tl() const;

        size_t get_size() {
            size_t culm_sum = contained_bodies.size();
        
            for(auto branch : branches) {
                if(branch)
                    culm_sum += branch->get_size();
            }

            return culm_sum;
        }

        void iterate(std::function<void(quad_tree_t* tree)> clbk) {
            clbk(this);

            for(auto branch : branches) {
                if(branch)
                    branch->iterate(clbk);
            }
        }

        size_t             depth;
        int32_t            index;
        quad_tree_type_e   type;
        world_tree_t*      root;
        quad_tree_t*       parent;
        aabb_pos_t                         area;
        memory_pool_t<const rigid_body_t*> contained_bodies;
        std::array<quad_tree_t*, 4>        branches;
    };

    class world_tree_t {
    public:
        world_tree_t(glm::vec2 area) {
            this->area.aabb = area;
            quad_tree_pool.column_capacity(1000);
        }

        bool insert(const rigid_body_t* body);
        void iterate(std::function<void(quad_tree_t*)>&& clbk);
        void remove(const rigid_body_t* body);
        bool relocate(const rigid_body_t* body);

        aabb_pos_t                         area;
        memory_pool_t<quad_tree_t>         quad_tree_pool;
        memory_pool_t<const rigid_body_t*> global_contained_bodies;
        hashmap_t<float, quad_tree_t*>     trees;
    };
}