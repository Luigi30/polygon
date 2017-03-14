#include "objects\sceneobject.hpp"

int SceneObject::ID = 0; //initialize the global counter

int SceneObject::getID(){
    return obj_ID; //This object's ID.
}

SceneObjectType SceneObject::getType(){
    return type;
}

void SceneObject::init(SceneObjectType _type){
    obj_ID = ++ID;

    type = _type;
    target = NULL;
    team = TEAM_NONE;
    is_dead = false;

    transformation.translation = Vector3f(0,0,0);
    transformation.rotation = Vector3f(0,0,0);
    transformation.scale = Vector3f(1,1,1);

    //just set this for development
    movement = MovementInfo();
    movement.enableDesiredRotation = false;
    movement.velocity = Vector3f(0,0,0);
    movement.forward_speed = 0.0f;
    movement.maximum_throttle_speed = 1.0f;
    max_rotational_velocity = Vector3f(0.5,0.5,0.5);

    throttle_percentage = 0;

    can_think = false;
}

void SceneObject::obj_think() {
    /* Behavior should be defined in derived classes. */
    assert(false);
    return;
}

Vector3f SceneObject::forward_vector(){
    Vector3f forward = Vector3f(0,0,1);
    forward = forward.rotateAroundYAxis(transformation.rotation.y);
    forward = forward.rotateAroundXAxis(transformation.rotation.x);
    forward = forward.rotateAroundZAxis(transformation.rotation.z);
    return forward;
}

Vector3f SceneObject::up_vector(){
    Vector3f up = Vector3f(0,1,0);
    up = up.rotateAroundYAxis(transformation.rotation.y);
    up = up.rotateAroundXAxis(transformation.rotation.x);
    up = up.rotateAroundZAxis(transformation.rotation.z);
    return up;
}

Vector3f SceneObject::right_vector(){
    Vector3f right = Vector3f(1,0,0);
    right = right.rotateAroundYAxis(transformation.rotation.y);
    right = right.rotateAroundXAxis(transformation.rotation.x);
    right = right.rotateAroundZAxis(transformation.rotation.z);
    return right;
}

void SceneObject::apply_velocity_to_translation() {
    transformation.translation = transformation.translation + movement.velocity;
}

float SceneObject::distance_to_target(){
    if(target == NULL){
        return -1; //no target
    }

    return Vector3f::distance(transformation.translation, target->transformation.translation);
}

void SceneObject::update_throttle_velocity(){
    movement.forward_speed = (throttle_percentage * max_forward_velocity);
    movement.velocity = forward_vector() * movement.forward_speed;
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
            transformation.rotation.x -= max_rotational_velocity.x;
        } else {
            transformation.rotation.x += max_rotational_velocity.x;  
        }
    }

    if(deltaY != 0){
        if(deltaY > 180.0f || deltaY < 0.0f) {
            transformation.rotation.y -= max_rotational_velocity.y;
        } else {
            transformation.rotation.y += max_rotational_velocity.y;  
        }     
    }

    if(deltaZ != 0){
        if(deltaZ > 180.0f || deltaZ < 0.0f) {
            transformation.rotation.z -= max_rotational_velocity.z;
        } else {
            transformation.rotation.z += max_rotational_velocity.z;  
        }     
    }

}