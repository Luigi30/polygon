#pragma once

//DumbShip follows chains of waypoints. It doesn't do anything else.

#include <algorith>
#include "defines.hpp"
#include "objects\sceneobject.hpp"
#include "fixed.hpp"

class DumbShip : public SceneObject {
    public:
    void obj_think();

    ~DumbShip() {};
    DumbShip(std::string _name, Vector3f _position, Vector3f _rotation) : SceneObject(SO_DUMBSHIP) {
        name = _name;
        can_think = true; //this is an active object

        /* Initial transformation */
        transformation.translation = _position;
        transformation.rotation = _rotation;

        /* Ship speed settings */
        forward_acceleration = 0.005;
        forward_deceleration = 0.005;
        max_forward_velocity = 0.02;
        
        /* Model settings */
        model = WavefrontObject("testship.3d");
        transformation.scale = Vector3f(0.5,0.5,0.5);
    }
};

typedef std::shared_ptr<DumbShip> ptr_DumbShip;