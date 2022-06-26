
add_library(spock_engine STATIC)

target_link_libraries(spock_engine PUBLIC spock_framework)
target_include_directories(spock_engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(spock_engine PRIVATE
    "engine.cpp"
    "systems.hpp" "systems.cpp")

target_sources(spock_engine PRIVATE
    "systems/renderer.hpp" "systems/renderer.cpp"
    "systems/tiles.hpp" "systems/tiles.cpp")

target_sources(spock_engine PUBLIC
    "engine.hpp")