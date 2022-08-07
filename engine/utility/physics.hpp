#pragma

#include "aabb.hpp"

/*
    physics utility

    Box2D takes care of allocating and handling and updating of physics objects, 
    this provides a simplicity for use standalone use alongside just GLFW and GLAD.

    but we are using FLECS and GLM, glm is a rather simple solve as we need 
    are implicit conversions. on the other hand FLECS is rather complicated. 
    The goal is to provide a all-in-one interface with components meaning not 
    having to make the user allocate an object outside of flecs and then add 
    it as a component to whatever enttiy.

    more problems arise transform_tt is no longer useful, as position and scale no longer change 
    the size of colliders or reflect the position of said colliders.

    FIX #1: Rigidbodies are now considered the primary component. 
    (primary component is a type of component that holds the position and 
    rotation [and in other cases scale]) This is the best solution to fix this, 
    meaning lets say the quad renderer needs to access the transform it will 
    need to iterate through all entites with a b2Body and a quad_renderer_tt component

    FIX #2: declaring the type of all box2d components as b2Object* 
    component solves  problems of having the two seperate allocaters. 
    For component creating OnAdd, and OnDestroy will be used. 
    (box2d is mostly a create and watch library being its a physics engine 
    so OnAdd and OnDestroy should be enough) But if we do use functions that 
    are only appilicable after creation, (i.e. ApplyForce AddImpulse type of functions) the 
    component will need to be get'ted from flecs and apply whatever you want to it  

    FIX #2 issue: having multiple shapes / fixtures on one entity will 
    invite problems unfortunately. As that will mean multiple compnents on 
    one entity creating uneeded complications, so the easy best thing to do is 
    put them all together in one data type. so we'll have one body per entity, 
    and multiple shapes and fixtures in one data type, 
    same thing for joints and what not.
*/

#define SFK_QUAD_REGION_BRANCH -1

namespace spk {
    // this is a tag to be added to be paired with a component with a rigidbody
    struct contains_transform_tt {};  

    constexpr auto NULL_HANDLE = nullptr;

    // flecs is dumb when it comes to component types as pointers, so we gotta do some shit like this ;p
    struct comp_b2Body { b2Body* body = nullptr; }; 
    
    /* add OnAdd and OnDestroy callbacks to a flecs world*/
    void add_box2d_components(flecs::world& ecs_world, b2World* phy_world);
}