#include "objects\dumbship.hpp"

void DumbShip::obj_think() {
    /* Just move forward in a straight line forever. */
    Vector3f forward = forward_vector();
    movement.velocity.z = forward.z * 0.01;
}