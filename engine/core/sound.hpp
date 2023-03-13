#pragma once

#include "base.hpp"
#include "utility/hashmap.hpp"

namespace spk {
    class sound_manager_t {
    public:
        uint32_t load_music(const char* file);
        uint32_t load_chunk(const char* file);

        Mix_Music* get_music(uint32_t id);
        Mix_Chunk* get_chunk(uint32_t id);

        bool is_music_id_valid(uint32_t id) {
            return music_map.find(id) != music_map.end();
        }

        bool is_chunk_id_valid(uint32_t id) {
            return chunk_map.find(id) != chunk_map.end();
        }

    private:    
        uint32_t counter = 0;
        hashmap_t<uint32_t, Mix_Music*> music_map;
        hashmap_t<uint32_t, Mix_Chunk*> chunk_map;
    };

    /**
     * @brief open and prepare audio to be played on an audio device
     * 
     * @param frequency the frequency to playback audio at (in Hz).
     * @param format audio format, one of SDL's AUDIO_* values.
     * @param number_channels number of channels (1 is mono, 2 is stereo, etc).
     * @param chunk_size audio buffer size in sample FRAMES
     */
    void open_audio(int frequency = MIX_DEFAULT_FREQUENCY, uint16_t format = MIX_DEFAULT_FORMAT, 
        int number_channels = MIX_DEFAULT_CHANNELS, int chunk_size = 1024);

    /**
     * @brief closes audio, this will cause all audio to stop playing immediately. open_audio must be called again
     * for any sounds to be played again via play_music() or play_chunk()
     * 
     */
    void close_audio();

    /**
     * @brief plays music that music_id is assigned to
     * 
     * @param music_id the id of the sound to paly
     * @param loops: the amount of times to repeat, -1 to play indefenitley
     */
    void play_music(uint32_t music_id, int loops);

    /**
     * @brief plays a sound chunk that chunk_id is assigned to
     * 
     * @param chunk_id the id of the sound chunk to play
     * @param loops: the amount of times to repeat, -1 to play indefenitley
     */
    void play_chunk(uint32_t chunk_id, int loops);
}