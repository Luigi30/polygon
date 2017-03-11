#pragma once
/*  SceneObject: The parent class of all objects in a 3D simulation. */

#include <string>
#include "objects\movement.hpp"
#include "shapes\wavefront.hpp"

struct Transformation {
    Vector3f translation;
    Vector3f rotation;
    Vector3f scale;
};

class SceneObject {
    public:
    bool can_think;

    std::string name;               /* The global identifier of this SceneObject. */
    WavefrontObject model;          /* The 3D model that represents this SceneObject, if applicable. */
    MovementInfo movement;          /* The velocity info for this SceneObject. */
    Transformation transformation;  /* The world transformation of this SceneObject. */

#pragma warning 549 10
    virtual ~SceneObject() {};
    virtual void obj_think();
#pragma warning 549 9

    SceneObject() {
        transformation.translation = Vector3f(0,0,0);
        transformation.rotation = Vector3f(0,0,0);
        transformation.scale = Vector3f(1,1,1);

        //just set this for development
        movement = MovementInfo();
        movement.enableDesiredRotation = false;
        movement.velocity = Vector3f(0,0,0);
        movement.maximum_rotation_per_frame = Vector3f(0.5,0.5,0.5);
        movement.forward_speed = 0.0f;
        movement.maximum_throttle_speed = 1.0f;

        can_think = false;
    }

    void apply_velocity_to_translation();
    Vector3f forward_vector();

    //Proceed toward desired_rotation.
    void update_rotation();
};

typedef std::shared_ptr<SceneObject> SceneObject_ptr;