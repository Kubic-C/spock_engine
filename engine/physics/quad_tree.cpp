#include "quad_tree.hpp"

namespace spk {
    quad_tree_t::quad_tree_t() {
        spk_trace(); 
        
        type = QUAD_TREE_TYPE_ROOT;
        area.pos = { 0.0f, 0.0f };
        area.aabb.hh = 2.0f;
        area.aabb.hw = 2.0f;

        branches[BOTTOM_LEFT]  = nullptr;
        branches[BOTTOM_RIGHT] = nullptr;
        branches[TOP_RIGHT]    = nullptr;
        branches[TOP_LEFT]     = nullptr;
    }

    quad_tree_t::~quad_tree_t() {
        spk_trace();

        for(auto branch : branches) {
            if(branch)
                root->quad_tree_pool.letgo(branch);
        }
        
        if(type != QUAD_TREE_TYPE_ROOT) {
            parent->branches[type] = nullptr;
        } else {
            root->trees.erase(index);
        }
    }

    bool quad_tree_t::can_fit(const aabb_pos_t& pos) {
        spk_trace(); 
        
        return contain(area, pos);
    }

    bool quad_tree_t::can_fit_branch(const aabb_pos_t& pos) {
        if(contain(bl(), pos)) {
            return true;
        } else if(contain(br(), pos)) {
            return true;
        } else if(contain(tr(), pos)) {
            return true;
        } else if(contain(tl(), pos)) { 
            return true;
        }

        return false;
    }


    bool quad_tree_t::insert(memory_pool_t<quad_tree_t>& pool, const rigid_body_t* body) {
        spk_trace(); 
        
        const rigid_body_t** list_ptr;
        
        if(!can_fit(body->get_aabb_pos()))
            return false;

        if(depth + 1 >= SPK_MAX_QUAD_TREE_DEPTH)
            goto add;

        if(contain(bl(), body->get_aabb_pos())) {
            return insert_to_branch(pool, bl(), BOTTOM_LEFT, body);

        } else if(contain(br(), body->get_aabb_pos())) {
            return insert_to_branch(pool, br(), BOTTOM_RIGHT, body);

        } else if(contain(tr(), body->get_aabb_pos())) {
            return insert_to_branch(pool, tr(), TOP_RIGHT, body);

        } else if(contain(tl(), body->get_aabb_pos())) { 
            return insert_to_branch(pool, tl(), TOP_LEFT, body);
        }

    add:

        list_ptr = contained_bodies.alloc();
        *list_ptr = body;

        spk_get_quad_tree_loc(body)->tree = this;
        spk_get_quad_tree_loc(body)->ptr  = list_ptr;

        return true;
    }

    void quad_tree_t::remove(const rigid_body_t* body) {
        spk_trace(); 
        
        quad_tree_location_t* loc = spk_get_quad_tree_loc(body);

        spk_assert(loc->ptr  != nullptr, "this body belongs to no tree; was it initialized correctly?");
        spk_assert(loc->tree == this, "cannot remove an element if it does not belong to this tree");

        contained_bodies.letgo(loc->ptr);
    }

    bool quad_tree_t::relocate(const rigid_body_t* body) {
        spk_trace(); 
        
        remove(body);

        return root->insert(body);
    }

    bool quad_tree_t::insert_to_branch(memory_pool_t<quad_tree_t>& pool, const aabb_pos_t& area, uint8_t index, const rigid_body_t* body) {
        spk_trace(); 

        if(branches[index] == nullptr) {
            branches[index] = pool.alloc();
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

    glm::vec2 quad_tree_t::half_size_of_half_size() const {
        spk_trace(); 
        
        return glm::vec2(area.aabb.hw / 2.0f, area.aabb.hh / 2.0f);
    }

    aabb_pos_t quad_tree_t::bl() const {
        spk_trace(); 
        
        aabb_pos_t bl_area;
        glm::vec2  half_size = half_size_of_half_size();

        bl_area.aabb = half_size;
        bl_area.pos  = area.pos + glm::vec2(-half_size.x, -half_size.y);

        return bl_area;
    }

    aabb_pos_t quad_tree_t::br() const {
        spk_trace(); 
        
        aabb_pos_t br_area;
        glm::vec2  half_size = half_size_of_half_size();

        br_area.aabb = half_size;
        br_area.pos  = area.pos + glm::vec2(half_size.x, -half_size.y);

        return br_area;
    }

    aabb_pos_t quad_tree_t::tr() const {
        spk_trace(); 
        
        aabb_pos_t tr_area;
        glm::vec2  half_size = half_size_of_half_size();

        tr_area.aabb = half_size;
        tr_area.pos  = area.pos + glm::vec2(half_size.x, half_size.y);

        return tr_area;
    }

    aabb_pos_t quad_tree_t::tl() const {
        spk_trace(); 
        
        aabb_pos_t tl_area;
        glm::vec2  half_size = half_size_of_half_size();

        tl_area.aabb = half_size;
        tl_area.pos  = area.pos + glm::vec2(-half_size.x, half_size.y);

        return tl_area;
    }

    float find_closest_boundry(float pos, float size) {
        const float back_offset = fmodf(pos, size);

        if(pos < 0) {
            const float front_offset = size - std::abs(back_offset);

            return pos - front_offset;
        } else { // pos > 0
            return pos - back_offset;
        }
    }

    bool world_tree_t::insert(const rigid_body_t* body) {
        spk_trace();

        const int    magic_width = 100;
        const aabb_t whole_area = area.aabb.vec2() * 2.0f;
        const float  xpos = find_closest_boundry(body->transform.pos.x, whole_area.hw);
        const float  ypos = find_closest_boundry(body->transform.pos.y, whole_area.hh);
        const float  index = (ypos * magic_width) + xpos; 
        // hashmaps cant handle (i.e. hash it) glm::vec2 so we convert it to a 1D index

        if(trees.find(index) == trees.end()) {
            trees[index] = quad_tree_pool.alloc();
            spk_assert(trees[index] != nullptr, "could not allocate quad tree");

            trees[index]->type   = QUAD_TREE_TYPE_ROOT;
            trees[index]->parent = nullptr;
            trees[index]->root   = this;
            trees[index]->index  = index;
            trees[index]->area   = area;
            trees[index]->depth  = 0;
            trees[index]->area.pos = glm::vec2(xpos, ypos) + area.aabb.vec2();
        }

        // if the body cannot be added to any tree, it must be over
        // the boundry of two or more trees
        if(!trees[index]->insert(quad_tree_pool, body)) {
            const rigid_body_t** list_ptr = global_contained_bodies.alloc();
            if(list_ptr == nullptr)
                return false;

            *list_ptr = body;

            spk_get_quad_tree_loc(body)->tree = nullptr;
            spk_get_quad_tree_loc(body)->ptr  = list_ptr;
        }

        return true;
    }

    void world_tree_t::iterate(std::function<void(quad_tree_t*)>&& clbk) {
        for(auto tree : trees) {
            tree.second->iterate(clbk);
        }
    }

    void world_tree_t::remove(const rigid_body_t* body) {
        quad_tree_location_t* loc = spk_get_quad_tree_loc(body);

        spk_assert(loc->ptr  != nullptr, "this body belongs to no tree; was it initialized correctly?");
        spk_assert(loc->tree == nullptr, "this body does not belong to the world tree");

        global_contained_bodies.letgo(loc->ptr);
    }

    bool world_tree_t::relocate(const rigid_body_t* body) {
        spk_trace();

        remove(body);

        return insert(body);
    }
}