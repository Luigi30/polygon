#pragma once

#include "objects\sceneobject.hpp"

class DumbShip : public SceneObject {
    public:
    void obj_think();

    ~DumbShip() {}
};

typedef std::shared_ptr<DumbShip> ptr_DumbShip;