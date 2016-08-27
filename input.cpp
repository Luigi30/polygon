#include "input.hpp"

void read_keyboard(){
    if(kbhit()){
        char key = getch();

        //Input axes
        if(key == 'a'){
            controlsState.direction = Vector3f(-1,0,0);
        }
        else if(key == 'd'){
            controlsState.direction = Vector3f(1,0,0);
        }
        else if(key == 'w'){
            controlsState.direction = Vector3f(0,0,1);
        }
        else if(key == 's'){
            controlsState.direction = Vector3f(0,0,-1);
        }
        else if (key == 'k'){
            controlsState.forward_throttle = std::min(controlsState.forward_throttle + 0.05, 1.0);
        }
        else if (key == ','){
            controlsState.forward_throttle = std::max(controlsState.forward_throttle - 0.05, 0.0);
        }
        else if(key == 'e'){
            controlsState.rotation.y = std::fmod(controlsState.rotation.y - ROTATION_DEGREES_PER_STEP, 360.0f);
        }
        else if(key == 'q'){
            controlsState.rotation.y = std::fmod(controlsState.rotation.y + ROTATION_DEGREES_PER_STEP, 360.0f);
        }
        else if(key == 'r'){
            controlsState.rotation.x = std::fmod(controlsState.rotation.x - ROTATION_DEGREES_PER_STEP, 360.0f);
        }
        else if(key == 'v'){
            controlsState.rotation.x = std::fmod(controlsState.rotation.x + ROTATION_DEGREES_PER_STEP, 360.0f);
        }
        else if(key == 'u'){
            controlsState.rotation.z = std::fmod(controlsState.rotation.z - ROTATION_DEGREES_PER_STEP, 360.0f);
        }
        else if(key == 'i'){
            controlsState.rotation.z = std::fmod(controlsState.rotation.z + ROTATION_DEGREES_PER_STEP, 360.0f);
        }

        //Special keys
        else if(key == 0x1B){
            controlsState.escapePressed = true;
        }

        //Debugging keys
        else if(key == 'f'){
            GAMEOBJECT("head")->transformation.rotation.y += 5.0;
        }
        else if (key == 'h'){
            GAMEOBJECT("head")->movement.desired_rotation = Vector3f(15, 30, 0);
        }
        else if (key == 'j'){
            GAMEOBJECT("head")->movement.desired_rotation = Vector3f(10, -25, 0);
        }

    } else {
        controlsState.direction = Vector3f(0,0,0); 
    }
}