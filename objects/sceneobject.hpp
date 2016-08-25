#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

#include <string>
#include "movement.hpp"
#include "..\includes\wavefront.hpp"

struct Transformation {
    Vector3f translation;
    Vector3f rotation;
    Vector3f scale;
};

class SceneObject {
    public:
    std::string name;
    WavefrontObject model;
    MovementInfo movement;

    //current
    Transformation transformation;

    SceneObject() {
        transformation.translation = Vector3f(0,0,0);
        transformation.rotation = Vector3f(0,0,0);
        transformation.scale = Vector3f(0,0,0);

        //just set this for development
        movement = MovementInfo();
        movement.enableDesiredRotation = false;
        movement.velocity = Vector3f(0,0,0);
        movement.maximum_rotation_per_frame = Vector3f(0.5,0.5,0.5);
        movement.forward_speed = 0.0f;
        movement.maximum_throttle_speed = 1.0f;
    }

    void apply_velocity_to_translation() {
        transformation.translation = transformation.translation + movement.velocity;
    }

    Vector3f forward_vector();

    //Proceed toward desired_rotation.
    void update_rotation();
};

#endif