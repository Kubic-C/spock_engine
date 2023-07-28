/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "systems/components.hpp"
#include "systems/window.hpp"
#include "systems/physics.hpp"
#include "systems/ps_tracker.hpp"
#include "systems/ui.hpp"
#include "systems/render.hpp"
#include "systems/primitive.hpp"
#include "systems/sprite.hpp"
#include "systems/camera.hpp"
#include "systems/character_controller.hpp"
#include "core/data.hpp"

namespace spk {
    /**
     * @brief set the vsync of window's buffer 
     * 
     * @param vsync the type of vsync to apply 
     */
    void vsync_set(vsync_setting_e vsync_opt);

    /**
     * @brief returns the name of the current build
     * 
     * @return name
     */
    const char* build_name();

    /**
     * @brief initializes the engine. Creates a default window, camera, rendering system,
     * opens the default audio channel, etc. . Should only be called once in the entire
     * lifetime of the application.
     * 
     */
    void init();

    /**
     * @brief runs the main loop, this is where the majority of the application's
     * runtime will be in. Essentially a while loop that ends when the current window's
     * exit button is hit or if exit() is called
     * 
     * @return an exit code, if non-zero some anomaly or error has occurred.
     */
    int  run();

    /**
     * @brief free's all of the engines resources
     * 
     */
    void free();

    /**
     * @brief prints the version numbers of most of the external libraries 
     * spock uses
     * 
     */
    void print_deps_versions();

    /**
     * @brief signals spock to exit.
     * 
     */
    void exit();
}