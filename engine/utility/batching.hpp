#pragma once

#include "framework.hpp"

/* TODO:
    implement static batching,
    implement dynamic batching (remove and add meshes)
*/

namespace spk {
template<size_t num>
    uint32_t find_first_bit_not_set(std::bitset<num> bits, uint32_t& start_index) {
        uint32_t index = start_index; 
        
        if(start_index != UINT32_MAX) {
            if(!bits[start_index]) {
                uint32_t new_last_possible_bit = start_index + 1;

                if(new_last_possible_bit < bits.size()) {
                    if(!bits[new_last_possible_bit]) {
                        start_index = new_last_possible_bit;
                    }
                } else {
                    start_index = UINT32_MAX;
                }

                return index;
            }
        } else { // start_index bit was not free'd
            do { // iterate through all bits starting from start index
                index++;

                if(index >= bits.size())
                    return UINT32_MAX;
            } while(bits[index]);

            if(index + 1 < bits.size()) {
                start_index = index + 1;
            }

            return index;
        }

        return UINT32_MAX; // satisfy warnings
    }

    class quad_batch_tt {
    public:
        void init(uint32_t max_quads);
        void free();

        uint32_t add(float x, float y);
        void set(uint32_t index, float x, float y);
        void remove(uint32_t index);

        uint32_t vertexes_size() {
            return quad_size * 6;
        }

        sfk::vertex_buffer_tt vertex_buffer;
        sfk::static_index_buffer_tt index_buffer;
    private:
        uint32_t quad_cap;
        uint32_t quad_size;
        
        /* in quads*/
        uint32_t vertex_buffer_size;
        uint32_t index_buffer_size; 

        uint32_t vertex_byte_size;
    };

    template<uint32_t max_size, uint32_t mesh_index_size>
    class quad_dynamic_batch_tt {
    public:
        bool init(uint32_t vertex_byte_size, uint32_t mesh_vertex_size);
        void free();

        uint32_t add();
        void set(uint32_t index, float* mesh);
        void remove(uint32_t index);

        uint32_t vertex_size() { return client_indexes.size(); }
        uint32_t vertexes()    { return vertex_buffer; }
        //uint32_t indexes()     { return client_indexes.size(); }
        void*    index_data()  { return client_indexes.data(); }

    private:
        uint32_t not_in_use;
        std::bitset<max_size> in_use;
        
        sfk::vertex_buffer_tt vertex_buffer;

        uint32_t vertex_byte_size; // the size of the vertex type in mesh in bytes
        uint32_t mesh_vertex_size; // the amount of vertexes per mesh

        //sfk::vertex_buffer_tt index_buffer;
        uint32_t index_buffer_size;

        std::vector<uint32_t> client_indexes;
    }; 

    template<uint32_t max_size, uint32_t mesh_index_size>
    bool quad_dynamic_batch_tt<max_size, mesh_index_size>::init(uint32_t vertex_byte_size, uint32_t mesh_vertex_size) {
        this->vertex_byte_size = vertex_byte_size;
        this->mesh_index_size  = mesh_index_size;
        this->mesh_vertex_size = mesh_vertex_size;
        this->index_buffer_size = 0;
        this->not_in_use = 0;

        const uint32_t mesh_byte_size = mesh_vertex_size * vertex_byte_size; 

        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data((max_size * mesh_byte_size), nullptr, GL_DYNAMIC_DRAW);

        // index_buffer.init(GL_ELEMENT_ARRAY_BUFFER);
        // index_buffer.buffer_data(max_size * mesh_index_size, nullptr, GL_DYNAMIC_DRAW);

        return true;
    }

    template<uint32_t max_size, uint32_t mesh_index_size>
    void quad_dynamic_batch_tt<max_size, mesh_index_size>::free() {
        //index_buffer.free();
        vertex_buffer.free();
    }

    template<uint32_t max_size, uint32_t mesh_index_size>
    uint32_t quad_dynamic_batch_tt<max_size, mesh_index_size>::add() {
        const uint32_t mesh_byte_size = mesh_vertex_size * vertex_byte_size; 
        uint32_t index;

        {
            index = find_first_bit_not_set(in_use, not_in_use);
            assert(index != UINT32_MAX && "could not add anymore meshes to batch, not enough space");
            in_use[index].flip();
        }

        { // generate the indexes based on index's position
            uint32_t vertex_position = index * mesh_vertex_size;
            //uint32_t index_position = index * mesh_index_size;
            
            std::vector<uint32_t> indexes;
            indexes.resize(mesh_index_size);

			indexes[0] = 0 + vertex_position;
			indexes[1] = 1 + vertex_position;
			indexes[2] = 2 + vertex_position;
			
			indexes[3] = 1 + vertex_position;
			indexes[4] = 2 + vertex_position;
			indexes[ 5] = 3 + vertex_position;   

            // index_buffer.buffer_sub_data(index_position * sizeof(uint32_t), indexes.size(), indexes.data());

            client_indexes.reserve(indexes.size());
            client_indexes.insert(client_indexes.end(), indexes.begin(), indexes.end());
        }

        return index;
    }
    
    template<uint32_t max_size, uint32_t mesh_index_size>
    void quad_dynamic_batch_tt<max_size, mesh_index_size>::set(uint32_t index, float* mesh) {
        const uint32_t mesh_byte_size = mesh_vertex_size * vertex_byte_size; 
        uint32_t byte_offset = mesh_byte_size * index;

        vertex_buffer.buffer_sub_data(byte_offset, mesh_byte_size, mesh);
    }
    
    template<uint32_t max_size, uint32_t mesh_index_size>
    void quad_dynamic_batch_tt<max_size, mesh_index_size>::remove(uint32_t index) {
        assert(in_use[index]);

        in_use[index].flip();

        { // remove the indexes from the index buffer
            uint32_t last_indexes = mesh_index_size * in_use.count();


        }
    } 
}