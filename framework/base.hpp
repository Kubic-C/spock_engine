#pragma once

// dependencies 
#include <flecs.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>

// extra
#include <cassert>
#include <stdint.h>

// streams
#include <iostream>
#include <fstream>
#include <sstream>

// containers
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <thread>
#include <array>
#include <memory>
#include <bitset>

// algorithms
#include <algorithm> 

namespace sfk {
    inline b2Vec2 to_glm_vec2(glm::vec2 vec2) { return (b2Vec2){vec2.x, vec2.y}; }
    inline glm::vec2 to_box_vec2(b2Vec2 vec2) { return (glm::vec2){vec2.x, vec2.y}; }
}

#define null NULL