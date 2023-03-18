add_compile_definitions(
    ecs_ftime_t=double)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(flecs)
add_subdirectory(SDL)

set(SDL2MIXER_MOD_MODPLUG OFF)
set(SDL2MIXER_OPUS OFF)
set(SDL2MIXER_FLAC OFF)
set(SDL2MIXER_MOD_XMP OFF)
set(SDL2MIXER_MIDI_FLUIDSYNTH OFF)
set(SDL2MIXER_WAVPACK OFF)
add_subdirectory(SDL_mixer)

add_subdirectory(glm)
add_subdirectory(glad/cmake)
add_subdirectory(freetype) 