#pragma once

#define MOUSE_INTERRUPT 0x33

#include <i86.h>

struct MouseData {
    int lmb_click;
    int rmb_click;
    int x;
    int y;    
};

class Mouse {

    public:
    static int isMouseAttached();
    static void cursorEnable();
    static void cursorDisable();
    static MouseData getMouseData();

};

int Mouse::isMouseAttached(){
    //INT 33h, AH=00h - reset mouse if attached.
    //out AX = 0x0000 if no mouse, 0xFFFF if mouse
    //out BX = number of buttons detected

    extern int __asmIsMouseAttached();
    #pragma aux __asmIsMouseAttached = "mov eax, 0x00" "int 0x33" value [eax] modify [eax];
    return __asmIsMouseAttached() > 0;
}

void Mouse::cursorEnable() {
    //INT 33h, AX=01h - enable
    #pragma aux __asmCursorEnable = "mov eax, 0x01" "int 0x33" modify [eax];
    __asmCursorEnable();
}    

void Mouse::cursorDisable(){
    //INT 33h, AX=02h - disable
    #pragma aux __asmCursorDisable = "mov eax, 0x02" "int 0x33" modify [eax];
    __asmCursorDisable();
}

MouseData Mouse::getMouseData(){
    //INT 33h, AH=03h
    MouseData mouseData;

    union REGS regs;
    regs.w.ax = 0x0003;
    int386(0x33, &regs, &regs);

    mouseData.lmb_click = regs.w.bx & 0x01 == 0x01;
    mouseData.rmb_click = regs.w.bx & 0x02 == 0x02;
    mouseData.x = regs.w.cx;
    mouseData.y = regs.w.dx;

    return mouseData;
}
