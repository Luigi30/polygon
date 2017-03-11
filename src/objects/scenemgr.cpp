#include "objects\scenemgr.hpp"

void SceneManager::addSceneObject(std::string _name, WavefrontObject _model, Vector3f _translation, Vector3f _rotation, Vector3f _scale){
    SceneObject_ptr obj_ptr(new SceneObject);
    obj_ptr->name = _name;
    obj_ptr->model = _model;
    obj_ptr->transformation.translation = _translation;
    obj_ptr->transformation.rotation = _rotation;
    obj_ptr->transformation.scale = _scale;    
    List_SceneObjects.push_back(obj_ptr);
}

void SceneManager::removeSceneObject(std::string _name){
    for(int i=0; i<List_SceneObjects.size(); i++){
        if(List_SceneObjects[i]->name == _name){
            List_SceneObjects.erase(List_SceneObjects.begin() + i);
            return;
        }
    }

    //error: trying to delete object that doesn't exist
    assert(false);
    return;
}

void SceneManager::addSceneObject(SceneObject_ptr _obj){
    List_SceneObjects.push_back(_obj);
}

SceneObject_ptr SceneManager::getSceneObjectPtr(std::string _name){
    for(int i=0;i<List_SceneObjects.size();i++){
        if(List_SceneObjects[i]->name == _name){
            return List_SceneObjects[i];
        }
    }
    
    //no object found
    assert(false);
    return List_SceneObjects[0]; //stop complaining, compiler
}

/*
SceneObject* SceneManager::getSceneObjectPtr(std::string _name){
    for(int i=0;i<List_SceneObjects.size();i++){
        if(List_SceneObjects[i]->name == _name){
            return &List_SceneObjects[i];
        }
    }
    
    //no object found
    assert(false);
    return NULL;
}
*/

void SceneManager::applyObjectVelocities(){
    for(int i=0;i<List_SceneObjects.size();i++){
        List_SceneObjects[i]->apply_velocity_to_translation();
    }
}

void SceneManager::applyObjectRotations(){
    for(int i=0;i<List_SceneObjects.size();i++){
        if(List_SceneObjects[i]->movement.enableDesiredRotation)
            List_SceneObjects[i]->update_rotation();
    }
}

void SceneManager::objectsThink(){
    for(int i=0;i<List_SceneObjects.size();i++){
        if(List_SceneObjects[i]->can_think == true){            
            List_SceneObjects[i]->obj_think();
        }
    } 
}