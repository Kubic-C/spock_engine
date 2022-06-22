#include <iostream>
#include <string>
#include "engine.hpp"

int main() {
    spk::engine engine;
    engine.init(1600, 900, "Cool 2D space game");

    engine.loop();

    engine.free();
    return 0;
}