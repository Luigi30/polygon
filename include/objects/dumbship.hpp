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
    DumbShip() : SceneObject(SO_DUMBSHIP) {
        forward_acceleration = 0.005;
        forward_deceleration = 0.005;
        max_forward_velocity = 0.04;
    }
};

typedef std::shared_ptr<DumbShip> ptr_DumbShip;