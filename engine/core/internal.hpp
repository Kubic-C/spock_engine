/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "data.hpp"

/* global accessible state | acting like a global context */

namespace spk {
    class engine_t;
    class render_system_t;
    class window_t;
    class canvas;

    /**
     * @brief contains all of the data that the spock engine uses.
     * 
     */
    struct internal_t {
        settings_t   settings;
        exit_info_t  exit_info;
        statistics_t statistics;
        scene_t      scene;
        resources_t  resources;
        allocators_t allocators;
    };

    extern internal_t* internal;
};