#include "input.hpp"

void read_keyboard(){
    int decayRequired = false;

    controlsState.direction = Vector3f(0,0,0);

    //Horizontal axis
    if(keyStates[KBD_A]){
        controlsState.direction.x = -1;
    }
    if(keyStates[KBD_D]){
        controlsState.direction.x = 1;
    }

    //Forward/backward
    if(keyStates[KBD_W]){
        controlsState.direction.z = 1;
    }
    if(keyStates[KBD_S]){
        controlsState.direction.z = -1;
    }

    //Horizontal rotation
    if(keyStates[KBD_Q]){
        controlsState.rotation.y = std::fmod(controlsState.rotation.y + ROTATION_DEGREES_PER_STEP, 360.0f);
    }
    if(keyStates[KBD_E]){
        controlsState.rotation.y = std::fmod(controlsState.rotation.y - ROTATION_DEGREES_PER_STEP, 360.0f);
    }

    while(kbhit()){
        char key = getch();

        //Input keys!
        if (key == 'k'){
            controlsState.forward_throttle = std::min(controlsState.forward_throttle + 0.05, 1.0);
        }
        else if (key == ','){
            controlsState.forward_throttle = std::max(controlsState.forward_throttle - 0.05, 0.0);
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

        else if(key == 'j'){
            //left rotation impulse - decays to 0 when released
        }
        else if(key == 'l'){
            //right rotation impulse - decays to 0 when released
        }

        //Special keys
        else if(key == 0x1B){
            controlsState.escapePressed = true;
        }

        //Debugging keys
        else if(key == 'f'){
            PTR_SCENEOBJECT("head")->transformation.rotation.y += 5.0;
        }
        else if (key == 'h'){
            PTR_SCENEOBJECT("head")->movement.desired_rotation = Vector3f(15, 30, 0);
        }
    }

    if(decayRequired){
        //controlsState.DecayAxes();
    }
}

void ControlsState::DecayAxes(){
    if(abs(direction.x > 0.1)){
        direction.x = direction.x * 0.7;
    } else {
        direction.x = 0;
    }

    if(abs(direction.y > 0.1)){
        direction.y = direction.y * 0.7;
    } else {
        direction.y = 0;
    }

    if(abs(direction.z > 0.1)){
        direction.z = direction.z * 0.7;
    } else {
        direction.z = 0;
    }
}

void _interrupt _far ISR_KBHook(void){
    static int extendedKey;

    unsigned char scancode = (unsigned char)LASTKEY();

    if(scancode == 0xE0)
        extendedKey = 1; //this is an extended key
    else {

        if(scancode & 0x80)
             (extendedKey ? extStates : keyStates)[scancode & 0x7f] = 0; //key up
        else 
             (extendedKey ? extStates : keyStates)[scancode] = 1;        //key down
        
        extendedKey = 0;
    }

    //chain interrupt to old hook
    ISR_OldKBHook();
}

void Install_KBHook(){
    ISR_OldKBHook = _dos_getvect(0x09);
    _dos_setvect(0x09, ISR_KBHook);
}

void Restore_KBHook(){
    _dos_setvect(0x09, ISR_OldKBHook);
}