#pragma once

#include <algorith>
#include "mouse.hpp"
#include "objects\scenemgr.hpp"
#include "shapes\vector3f.hpp"
#include "raster\screen.hpp"

#define PORT_KBD    0x60
#define LASTKEY()   inp(PORT_KBD)

volatile char keyStates[128]; /* array of key states           */
volatile char extStates[128]; /* array of AT key states        */

void (_interrupt _far *ISR_OldKBHook)(); //the old ISR for a key press/release
void _interrupt _far ISR_KBHook();       //the new ISR

#define ROTATION_DEGREES_PER_STEP 5

struct ControlsState {
    //Control axes will be between -1.0 and 1.0
    float pitch;
    float yaw;
    float roll;

    Vector3f direction;
    Vector3f rotation;
    float forward_throttle;

    bool escapePressed;

    void DecayAxes();
};

void Install_KBHook();
void Restore_KBHook();

ControlsState controlsState;

void read_keyboard();
void INPUT_DecayAxes();

//Keyboard scancodes
#define KBD_A   0x1E
#define KBD_D   0x20
#define KBD_E   0x12
#define KBD_Q   0x10
#define KBD_S   0x1F
#define KBD_W   0x11
