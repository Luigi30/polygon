/* A big mess of defines that need to be reorganized. */
#pragma once

#include <conio.h>
#include <i86.h>
#include <string>

#include "colors.hpp"

double PI = 3.14159265;

//VGA
#define TEXTCELL(X,Y) Point(X, Y*8)
#define VGA_MEMORY 0xA000
#define VGA_LINEAR_ADDRESS ((VGA_MEMORY) << 4)
#define VGA_HEIGHT 200
#define VGA_WIDTH 320
#define VGA_SIZE (VGA_HEIGHT * VGA_WIDTH)
int VGA_Y_OFFSETS[VGA_HEIGHT];
float SIN_TABLE[360];
float COS_TABLE[360];
char *VGA_PTR;

//Timer
#define TICKS_PER_SECOND 240
#define TICKS_PER_FRAME (TICKS_PER_SECOND / 60)

//Debug output
void debugOutput(std::string str){
    for(int i=0;i<str.length();i++){
        outp(0x3F8, str[i]); //COM1
        delay(1);
    }
}
#define DEBUG(X) serialPort.sendString(X);

/******** Common Type Definitons ********/
// unsigned types
typedef unsigned char         Byte;
typedef unsigned short        UInt16;
typedef unsigned long         UInt32;
typedef unsigned long long    UInt64;
// signed types
typedef signed char           SByte;
typedef signed short          Int16;
typedef signed long           Int32;
typedef signed long long      Int64;

typedef int                   COLOR;

//Button shape
enum ButtonShape { BUTTON_SHAPE_RECT, BUTTON_SHAPE_ROUNDRECT };
