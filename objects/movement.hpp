#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "..\shapes\vector3f.hpp"

class MovementInfo {
    public:
    float forward_speed;
    Vector3f velocity;
    bool alwaysRemainUpright;
    bool enableDesiredRotation;

    //desired
    Vector3f desired_rotation;
    Vector3f maximum_rotation_per_frame; //degrees per frame
};

#endif