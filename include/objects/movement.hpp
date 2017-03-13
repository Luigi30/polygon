#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "..\shapes\vector3f.hpp"

struct MovementInfo {
    float forward_speed;
    Vector3f velocity;
    bool alwaysRemainUpright;
    bool enableDesiredRotation;
    int maximum_throttle_speed;

    Vector3f desired_rotation;
};

#endif