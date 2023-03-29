/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sound.hpp"
#include "core/internal.hpp"

namespace spk {
    uint32_t sound_dictionary_t::chunk_add(Mix_Chunk* chunk) {
        chunk_map[++counter] = chunk;

        return counter;
    }

    uint32_t sound_dictionary_t::music_add(Mix_Music* music) {
        music_map[++counter] = music;

        return counter;
    }

    Mix_Music* sound_dictionary_t::music_get(uint32_t id) {
        if(!is_music_id_valid(id))
            return nullptr;

        return music_map[id];
    }

    Mix_Chunk* sound_dictionary_t::chunk_get(uint32_t id) {
        if(!is_chunk_id_valid(id))
            return nullptr;

        return chunk_map[id];
    }

    bool sound_dictionary_t::is_music_id_valid(uint32_t id) {
        return music_map.find(id) != music_map.end();
    }

    bool sound_dictionary_t::is_chunk_id_valid(uint32_t id) {
        return chunk_map.find(id) != chunk_map.end();
    }

    sound_dictionary_t& sound_dictionary() {
        return internal->resources.sounds;
    }

    void audio_open(int frequency, Uint16 format, int number_channels, int chunk_size) {
        int code = Mix_OpenAudio(frequency, format, number_channels, chunk_size);
        if(code == -1)
            log.log(LOG_TYPE_INFO, "[red, em]could not[reset] open audio with %iHz / (format) %u / (channels) %i / (chunk size) %i", frequency, format, number_channels, chunk_size);
        
    }

    void audio_close() {
        Mix_CloseAudio();
    }

    uint32_t music_create(const char* file) {
        Mix_Music* music = Mix_LoadMUS(file);
        if(music == nullptr) {
            return UINT32_MAX;
        }

        return sound_dictionary().music_add(music);
    }

    uint32_t chunk_create(const char* file) {
        Mix_Chunk* chunk = Mix_LoadWAV(file);
        if(chunk == nullptr) {
            return UINT32_MAX;
        }

        return sound_dictionary().chunk_add(chunk);
    }

    void music_play(uint32_t sound_id, int loops) {
        Mix_Music* music = sound_dictionary().music_get(sound_id);

        if(music == nullptr)
            return;

        Mix_PlayMusic(music, loops);
    }

    void chunk_play(uint32_t chunk_id, int loops, int channel) {
        Mix_Chunk* chunk = sound_dictionary().chunk_get(chunk_id);
        
        if(chunk == nullptr)
            return;

        Mix_PlayChannel(channel, chunk, loops);
    }
}