#include "sceneobject.hpp"

Vector3f SceneObject::forward_vector(){
    Vector3f forward = Vector3f(0,0,-1);
    forward = forward.rotateAroundXAxis(rotation.x);
    forward = forward.rotateAroundYAxis(rotation.y);
    forward = forward.rotateAroundZAxis(rotation.z);
    return forward;
}

void SceneObject::update_rotation() {

    //Clamp rotation to a range between 0 and 360 degrees
    if(rotation.x < 0.0f){
        rotation.x = rotation.x + 359.0f;
    }
    if(rotation.y < 0.0f){
        rotation.y = rotation.y + 359.0f;
    }
    if(rotation.z < 0.0f){
        rotation.z = rotation.z + 359.0f;
    }

    // how far are we from the desired rotation?
    if(desired_rotation.x < 0){
        desired_rotation.x += 360.0f;
    }
    if(desired_rotation.y < 0){
        desired_rotation.y += 360.0f;
    }
    if(desired_rotation.z < 0){
        desired_rotation.z += 360.0f;
    }

    float deltaX = desired_rotation.x - rotation.x;
    float deltaY = desired_rotation.y - rotation.y;
    float deltaZ = desired_rotation.z - rotation.z;

    if(deltaX != 0){
        if(deltaX > 180.0f || deltaX < 0.0f) {
            rotation.x -= maximum_rotation_per_frame.x;
        } else {
            rotation.x += maximum_rotation_per_frame.x;  
        }
    }

    if(deltaY != 0){
        if(deltaY > 180.0f || deltaY < 0.0f) {
            rotation.y -= maximum_rotation_per_frame.y;
        } else {
            rotation.y += maximum_rotation_per_frame.y;  
        }     
    }

    if(deltaZ != 0){
        if(deltaZ > 180.0f || deltaZ < 0.0f) {
            rotation.z -= maximum_rotation_per_frame.z;
        } else {
            rotation.z += maximum_rotation_per_frame.z;  
        }     
    }

}