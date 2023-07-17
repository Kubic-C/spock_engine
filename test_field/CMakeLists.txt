
add_executable(spock_engine_test)

target_link_libraries(spock_engine_test PUBLIC spock_engine)

target_sources(spock_engine_test PUBLIC main.cpp)