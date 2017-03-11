#pragma once
#include "objects\sceneobject.hpp"

#define PTR_SCENEOBJECT(X) g_SceneManager.getSceneObjectPtr(X)

class SceneManager {
    std::vector<SceneObject_ptr> List_SceneObjects;
    
    /* Methods */
    public:
    void addSceneObject(std::string _name, WavefrontObject _model, Vector3f _translation, Vector3f _rotation, Vector3f _scale);
    void addSceneObject(SceneObject_ptr _obj);
    void removeSceneObject(std::string _name);
    std::vector< std::shared_ptr<SceneObject> > getSceneObjectsList() { return List_SceneObjects; };

    void SceneManager::applyObjectVelocities();
    void SceneManager::applyObjectRotations();

    void SceneManager::objectsThink();

    //SceneObject* getSceneObjectPtr(std::string _name);
    SceneObject_ptr getSceneObjectPtr(std::string _name);
};

SceneManager g_SceneManager;