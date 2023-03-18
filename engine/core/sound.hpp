/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"
#include "utility/hashmap.hpp"

namespace spk {
    /**
     * @brief holds a list of sounds that can be accessed via an ID
     * 
     */
    class sound_dictionary_t {
    public:
        uint32_t chunk_add(Mix_Chunk* chunk);
        uint32_t music_add(Mix_Music* music);

        Mix_Music* music_get(uint32_t id);
        Mix_Chunk* chunk_get(uint32_t id);

        bool is_music_id_valid(uint32_t id);
        bool is_chunk_id_valid(uint32_t id);

    private:
        // a map of aliases
        hashmap_t<uint32_t, Mix_Music*> music_map;
        hashmap_t<uint32_t, Mix_Chunk*> chunk_map;

        uint32_t counter = 0; // increments by one everytime a chunk or music is added
    };

    sound_dictionary_t& sound_dictionary();

    /**
     * @brief open and prepare audio to be played on an audio device
     * 
     * @param frequency the frequency to playback audio at (in Hz).
     * @param format audio format, one of SDL's AUDIO_* values.
     * @param number_channels number of channels (1 is mono, 2 is stereo, etc.)
     * @param chunk_size audio buffer size in sample FRAMES
     */
    void audio_open(int frequency = MIX_DEFAULT_FREQUENCY, uint16_t format = MIX_DEFAULT_FORMAT, 
        int number_channels = MIX_DEFAULT_CHANNELS, int chunk_size = 1024);

    /**
     * @brief closes audio, this will cause all audio to stop playing immediately. open_audio must be called again
     * for any sounds to be played again
     * 
     */
    void audio_close();

    /**
     * @brief load music from a file and returns the ID of the music loaded
     * 
     * @param file the sound file to load
     * 
     * @return returns an ID number; if this number is UINT32_MAX the file could not be
     * loaded or the music could not be created.
     */
    uint32_t music_load(const char* file);

    /**
     * @brief plays music that music_id is assigned to
     * 
     * @param music_id the id of the sound to paly
     * @param loops: the amount of times to repeat, -1 to play indefenitley
     */
    void music_play(uint32_t music_id, int loops);

    /**
     * @brief load chunk from a file and returns the ID of the chunk loaded
     * 
     * @param file the sound file to load
     * 
     * @return returns an ID number; if this number is UINT32_MAX the file could not be
     * loaded or the chunk could not be created.
     */
    uint32_t chunk_load(const char* file);

    /**
     * @brief plays a sound chunk that chunk_id is assigned to
     * 
     * @param chunk_id the id of the sound chunk to play
     * @param loops: the amount of times to repeat, -1 to play indefenitley
     * @param channel: the channel to play on, -1 to play on the first available channel
     */
    void chunk_play(uint32_t chunk_id, int loops, int channel = -1);
}