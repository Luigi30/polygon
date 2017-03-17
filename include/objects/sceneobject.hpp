#pragma once
/*  SceneObject: The parent class of all objects in a 3D simulation. */

#include <string>
#include "objects\behavior.hpp"
#include "objects\movement.hpp"
#include "shapes\wavefront.hpp"

typedef enum Team_t {
    TEAM_NONE,
    TEAM_GOOD,
    TEAM_BAD,
    TEAM_NEUTRAL
} Team;

typedef enum SceneObjectType_t {
    SO_GENERIC,
    SO_PLAYER,
    SO_DUMBSHIP,
    SO_WAYPOINT
} SceneObjectType;

struct Transformation {
    Vector3f translation;
    Vector3f rotation;
    Vector3f scale;
};

class SceneObject {
    private:
    static int ID;  //Globally unique ID for each SceneObject
    int obj_ID;     //this SceneObject's ID
    SceneObjectType type; //this SceneObject's type

    public:
    bool can_think;

    std::string name;               /* The global identifier of this SceneObject. */
    WavefrontObject model;          /* The 3D model that represents this SceneObject, if applicable. */
    MovementInfo movement;          /* The velocity info for this SceneObject. */
    Transformation transformation;  /* The world transformation of this SceneObject. */

    Team team;
    bool is_dead;

    /* AI stuff */
    Behavior    current_behavior;   /* What behavior is this SceneObject doing? */
    SceneObject *target;            /* What's the target of this SceneObject's current goal? */

    /* Velocity and acceleration info */
    float throttle_percentage;
    Vector3f max_rotational_velocity;   /* Degrees per frame this object will rotate */
    float max_forward_velocity;         /* Throttle is expressed as a percentage of this value */
    float forward_acceleration;         /* How much speed does this object gain each frame? */
    float forward_deceleration;         /* How much speed does this object lose each frame? */

#pragma warning 549 10
    virtual ~SceneObject() {};
    virtual void obj_think();
#pragma warning 549 9

    SceneObject(SceneObjectType _type){
        init(_type);
    }

    SceneObject() {
        init(SO_GENERIC);
    }

    void init(SceneObjectType _type);

    Vector3f forward_vector();
    Vector3f up_vector();
    Vector3f right_vector();

    int getID();
    SceneObjectType getType();
    void update_throttle_velocity();
    void apply_velocity_to_translation();

    void update_rotation();
    float distance_to_target();
};

typedef std::shared_ptr<SceneObject> SceneObject_ptr;