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
        // important to note: the order of the members DO matter.
        // It affects in which order each member's constructor and deconstructor
        // is called, so it necessary that they must be in this order

        allocators_t allocators;
        resources_t  resources;
        exit_info_t  exit_info;
        settings_t   settings;
        statistics_t statistics;
        scene_t      scene;
    };

    extern internal_t* internal;
};