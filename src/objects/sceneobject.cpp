#include "objects\sceneobject.hpp"

void SceneObject::obj_think() {
    /* Behavior should be defined in derived classes. */
    assert(false);
    return;
}

Vector3f SceneObject::forward_vector(){
    Vector3f forward = Vector3f(0,0,-1);
    forward = forward.rotateAroundYAxis(transformation.rotation.y);
    forward = forward.rotateAroundXAxis(transformation.rotation.x);
    forward = forward.rotateAroundZAxis(transformation.rotation.z);
    return forward;
}

void SceneObject::apply_velocity_to_translation() {
    transformation.translation = transformation.translation + movement.velocity;
}

void SceneObject::update_rotation() {

    //Clamp rotation to a range between 0 and 360 degrees
    if(transformation.rotation.x < 0.0f){
        transformation.rotation.x = transformation.rotation.x + 359.0f;
    }
    if(transformation.rotation.y < 0.0f){
        transformation.rotation.y = transformation.rotation.y + 359.0f;
    }
    if(transformation.rotation.z < 0.0f){
        transformation.rotation.z = transformation.rotation.z + 359.0f;
    }

    // how far are we from the desired rotation?
    if(movement.desired_rotation.x < 0){
        movement.desired_rotation.x += 360.0f;
    }
    if(movement.desired_rotation.y < 0){
        movement.desired_rotation.y += 360.0f;
    }
    if(movement.desired_rotation.z < 0){
        movement.desired_rotation.z += 360.0f;
    }

    float deltaX = movement.desired_rotation.x - transformation.rotation.x;
    float deltaY = movement.desired_rotation.y - transformation.rotation.y;
    float deltaZ = movement.desired_rotation.z - transformation.rotation.z;

    if(deltaX != 0){
        if(deltaX > 180.0f || deltaX < 0.0f) {
            transformation.rotation.x -= movement.maximum_rotation_per_frame.x;
        } else {
            transformation.rotation.x += movement.maximum_rotation_per_frame.x;  
        }
    }

    if(deltaY != 0){
        if(deltaY > 180.0f || deltaY < 0.0f) {
            transformation.rotation.y -= movement.maximum_rotation_per_frame.y;
        } else {
            transformation.rotation.y += movement.maximum_rotation_per_frame.y;  
        }     
    }

    if(deltaZ != 0){
        if(deltaZ > 180.0f || deltaZ < 0.0f) {
            transformation.rotation.z -= movement.maximum_rotation_per_frame.z;
        } else {
            transformation.rotation.z += movement.maximum_rotation_per_frame.z;  
        }     
    }

}