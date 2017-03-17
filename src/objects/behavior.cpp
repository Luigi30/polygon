#include "objects\behavior.hpp"
#include "objects\sceneobject.hpp"
#include "raster\screen.hpp"

void AI_follow_waypoints(SceneObject *obj, SceneObject *waypoint) {

    Vector3f v1 = obj->forward_vector();
    Vector3f v2 = waypoint->transformation.translation;
    Quaternion q = Quaternion::fromTwoVectors(v1, v2);

    obj->transformation.rotation = q.toVector3f(Q_DEGREES);

    //Move toward the object.
    if(obj->distance_to_target() > 1.0f){     
        obj->throttle_percentage = 1;
    } else {
        //We reached the waypoint.
        if(waypoint->target != NULL){
            obj->target = waypoint->target; //Reset for the next waypoint
        } else {
            obj->throttle_percentage = 0; //Out of waypoints
        }
    }

    obj->update_throttle_velocity();

    /* Follow a set of waypoints. */

    /*
    Vector3f orientation = obj->forward_vector();
    Vector3f destination_position = waypoint->transformation.translation;

    Quaternion initial_rotation = Quaternion(1, 0, 0, 0);
    Quaternion q = Quaternion::RotationBetweenVectors(orientation, destination_position);
    Vector3f required_rotation = q.toVector3f(Q_DEGREES);

    Quaternion q_obj = Quaternion(obj->transformation.rotation);

    Vector3f target_position = waypoint->transformation.translation;
    float maximum_rotation_degrees = 2.5;

    float x_distance = target_position.x - obj->transformation.translation.x;
    float y_distance = target_position.y - obj->transformation.translation.y;
    float z_distance = target_position.z - obj->transformation.translation.z;
    float xz_distance = std::sqrt((x_distance * x_distance) + (z_distance * z_distance));

    float x_desired_rotation = R2D(-std::atan2(y_distance, xz_distance));
    float y_desired_rotation = R2D(std::atan2(x_distance, z_distance));

    float x_required_rotation = x_desired_rotation - obj->transformation.rotation.x;
    float y_required_rotation = y_desired_rotation - obj->transformation.rotation.y;

    char distanceText[80];
    sprintf(distanceText, "REQD X: %f", x_required_rotation);
    g_screen.layer_text.putString(distanceText, strlen(distanceText), TEXT_XY(0,7), COLOR_GREEN, FONT_4x6);

    float x_this_frame_rotation = 0;
    float y_this_frame_rotation = 0;

    if(std::abs(x_required_rotation) > 5)
    {
        if (x_required_rotation > 0) {
            x_this_frame_rotation += std::fmin(maximum_rotation_degrees, std::abs(x_required_rotation));
        }
        else {
            x_this_frame_rotation -= std::fmin(maximum_rotation_degrees, std::abs(x_required_rotation));
        }

        obj->transformation.rotation.x = obj->transformation.rotation.x + x_this_frame_rotation; //around Y
    }

    if(std::abs(y_required_rotation) > 1)
    {
        if (y_required_rotation > 0) {
            y_this_frame_rotation += std::fmin(maximum_rotation_degrees, std::abs(y_required_rotation));
        }
        else {
            y_this_frame_rotation -= std::fmin(maximum_rotation_degrees, std::abs(y_required_rotation));
        }

        //obj->transformation.rotation.y = obj->transformation.rotation.y + y_this_frame_rotation; //around X
    }

    obj->transformation.rotation.z = 0;

    //Move toward the object.
    if(obj->distance_to_target() > 1.0f){     
        obj->throttle_percentage = 1;
    } else {
        //We reached the waypoint.
        if(waypoint->target != NULL){
            obj->target = waypoint->target; //Reset for the next waypoint
        } else {
            obj->throttle_percentage = 0; //Out of waypoints
        }
    }

    obj->update_throttle_velocity();
    */
}