/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once 

#include "base.hpp"

// per second tracker | UPS TPS DELTA-TIME

namespace spk {
    void ps_tracker_system_init(flecs::world& world);
}