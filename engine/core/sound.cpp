#include "sound.hpp"
#include "core/internal.hpp"

namespace spk {
    uint32_t   sound_manager_t::load_music(const char* file) {
        Mix_Music* music = Mix_LoadMUS(file);
        if(music == nullptr) {
            return UINT32_MAX;
        }

        music_map[++counter] = music;

        return counter;
    }

    uint32_t   sound_manager_t::load_chunk(const char* file) {
        Mix_Chunk* chunk = Mix_LoadWAV(file);
        if(chunk == nullptr) {
            return UINT32_MAX;
        }

        chunk_map[++counter] = chunk;

        return counter;
    }

    Mix_Music* sound_manager_t::get_music(uint32_t id) {
        if(!is_music_id_valid(id))
            return nullptr;

        return music_map[id];
    }

    Mix_Chunk* sound_manager_t::get_chunk(uint32_t id) {
        if(!is_chunk_id_valid(id))
            return nullptr;

        return chunk_map[id];
    }

    void open_audio(int frequency, Uint16 format, int number_channels, int chunk_size) {
        int code = Mix_OpenAudio(frequency, format, number_channels, chunk_size);
        if(code == -1)
            log.log(LOG_TYPE_INFO, "[red, em]could not[reset] open audio with %iHz / (format) %u / (channels) %i / (chunk size) %i", frequency, format, number_channels, chunk_size);
        
    }

    void close_audio() {
        Mix_CloseAudio();
    }

    void play_music(uint32_t sound_id, int loops) {
        Mix_Music* music = internal->resources.sounds.get_music(sound_id);
        int code;
        
        if(music == nullptr)
            return;

        code = Mix_PlayMusic(music, loops);
        if(code == -1)
            spk::log.log(LOG_TYPE_INFO, "could not play music: %s",  Mix_GetError());
    }

    void play_chunk(uint32_t chunk_id, int loops) {
        Mix_Chunk* chunk = internal->resources.sounds.get_chunk(chunk_id);
        int code;
        
        if(chunk == nullptr)
            return;

        code = Mix_PlayChannel(-1, chunk, loops);
        if(code == -1)
            spk::log.log(LOG_TYPE_INFO, "could not play chunk: %s",  Mix_GetError());
    }
}