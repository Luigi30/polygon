#ifndef INPUT_HPP
#define INPUT_HPP

#include <algorith>
#include "shapes\vector3f.hpp"
#include "screen.hpp"

#define ROTATION_DEGREES_PER_STEP 5

struct ControlsState {
    //Control axes will be between -1.0 and 1.0
    float pitch;
    float yaw;
    float roll;

    Vector3f direction;
    Vector3f rotation;
    float forward_throttle;

    bool escapePressed;
};

ControlsState controlsState;

void read_keyboard();

#endif  INPUT_HPP