#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

#include <string>
#include "..\includes\wavefront.hpp"

class SceneObject {
    public:
    std::string name;
    WavefrontObject model;

    //current
    Vector3f translation;
    Vector3f rotation;
    Vector3f scale;

    //desired
    Vector3f desired_rotation;
    Vector3f maximum_rotation_per_frame; //degrees per frame
    
    float forward_speed;
    Vector3f velocity;

    SceneObject() {
        translation = Vector3f(0,0,0);
        rotation = Vector3f(0,0,0);
        scale = Vector3f(0,0,0);
        velocity = Vector3f(0,0,0);
        
        //just set this for development
        maximum_rotation_per_frame = Vector3f(0.5,0.5,0.5);
        forward_speed = 0.0f;
    }

    void apply_velocity_to_translation() {
        translation = translation + velocity;
    }

    Vector3f forward_vector();

    //Proceed toward desired_rotation.
    void update_rotation();
};

#endif