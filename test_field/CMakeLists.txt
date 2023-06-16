
add_executable(test_field)

target_link_libraries(test_field PUBLIC spock_engine)

target_sources(test_field PUBLIC main.cpp)