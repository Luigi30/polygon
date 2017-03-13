#include "objects\dumbship.hpp"

void DumbShip::obj_think() {
    AI_follow_waypoints(this, this->target);
}