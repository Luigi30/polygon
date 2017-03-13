#pragma once

#include "defines.hpp"
#include "shapes\vector3f.hpp"

class SceneObject;

typedef enum Behavior_t {
    BEHAVIOR_WAYPOINTS
} Behavior;

void AI_follow_waypoints(SceneObject *obj, SceneObject *waypoint);