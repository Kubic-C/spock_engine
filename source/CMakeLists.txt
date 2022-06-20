
add_executable(space_game2d)

target_link_libraries(space_game2d PUBLIC spock_engine)
target_include_directories(space_game2d PUBLIC "../depend")

target_sources(space_game2d PUBLIC main.cpp)