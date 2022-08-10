
add_subdirectory(flecs)
add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(glm)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests")
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed")
add_subdirectory(box2d)

add_subdirectory(freetype)