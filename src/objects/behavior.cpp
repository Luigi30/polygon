#include "objects\behavior.hpp"
#include "objects\sceneobject.hpp"

void AI_follow_waypoints(SceneObject *obj, SceneObject *waypoint) {
    /* Follow a set of waypoints. */

    Vector3f target_position = waypoint->transformation.translation;
    float maximum_rotation_degrees = 2.0;

    float x_distance = target_position.x - obj->transformation.translation.x;
    float y_distance = target_position.y - obj->transformation.translation.y;
    float z_distance = target_position.z - obj->transformation.translation.z;
    float xz_distance = std::sqrt(x_distance * x_distance + z_distance * z_distance);

    float x_desired_rotation = R2D(std::atan2(y_distance, xz_distance));
    float y_desired_rotation = R2D(std::atan2(x_distance, z_distance));

    float x_required_rotation = x_desired_rotation - obj->transformation.rotation.x;
    float y_required_rotation = y_desired_rotation - obj->transformation.rotation.y;

    float x_this_frame_rotation = 0;
    float y_this_frame_rotation = 0;

    if (x_required_rotation > 0) {
        x_this_frame_rotation += std::fmin(maximum_rotation_degrees, std::abs(x_required_rotation));
    }
    else {
        x_this_frame_rotation -= std::fmin(maximum_rotation_degrees, std::abs(x_required_rotation));
    }

    if (y_required_rotation > 0) {
        y_this_frame_rotation += std::fmin(maximum_rotation_degrees, std::abs(y_required_rotation));
    }
    else {
        y_this_frame_rotation -= std::fmin(maximum_rotation_degrees, std::abs(y_required_rotation));
    }

    obj->transformation.rotation.x = obj->transformation.rotation.x + x_this_frame_rotation; //around Y
    obj->transformation.rotation.y = obj->transformation.rotation.y + y_this_frame_rotation; //around X
    obj->transformation.rotation.z = 0;

    /* Move toward the object. */
    if(obj->distance_to_target() > 1.0f){     
        obj->throttle_percentage = 1;
    } else {
        /* We reached the waypoint. */
        if(waypoint->target != NULL){
            obj->target = waypoint->target; //Reset for the next waypoint
        } else {
            obj->throttle_percentage = 0; //Out of waypoints
        }
    }

    obj->update_throttle_velocity();
}