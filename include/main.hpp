#pragma once

#define TRUE true
#define FALSE false

#include <cstdio>
#include <vector>
#include <iostream>
#include <bios.h>

#include "dos.h"

#include "serial.hpp"
#include "objects\scenemgr.hpp"
#include "objects\dumbship.hpp"
#include "raster\rasterize.hpp"
#include "raster\screen.hpp"
#include "shapes\wavefront.hpp"
#include "widgets\button.hpp"
#include "input.hpp"
#include "fixed.hpp"
#include <cmath>
#include <time.h>

#define M_PI 3.141592653589793238462643383279502884
#define DEG_TO_RAD(X) ((X * M_PI) / 180.0)

std::vector<Shape> shapesList;

//Timers
void __interrupt __far timerHandler();
void (__interrupt __far *biosTimerHandler)();
volatile unsigned long biosClockTicks;
unsigned long timerReload;
volatile unsigned long myTimerTicks;
volatile unsigned int timer60Hz;
volatile unsigned int timer24Hz;

bool DoSceneLoop();
void MainMenu();
void BeginSimulation();

int dpmi_set_pm_handler(unsigned char intnum, void far * isr )
{
        union REGS regs;

        /* Use DPMI call 204h to get pm interrrupt */
        std::memset(&regs,0,sizeof(regs));
        regs.w.ax = 0x0205;
        regs.h.bl = intnum;
        regs.w.cx = FP_SEG(isr);
        regs.x.edx = FP_OFF(isr);
        int386( 0x31, &regs, &regs );
        if (!regs.w.cflag)      
                return 0;
        return 1;
}
/*
// PIT interrupt handler
void __interrupt __far timerHandler () {
    //This will fire at a rate set in init_timer()

    for(int i=0;i<8;i++){
        timerList[i].tick();
    }

    //midiPlayer.processEvents();
    
    // increment the tick counter by the value we loaded into the timer
    biosClockTicks += timerReload;
    // should we also call the BIOS handler?
    if ( biosClockTicks >= 0x10000 ) {
        // decrement the value
        biosClockTicks -= 0x10000;
      
        // call the BIOS handler
        _chain_intr( biosTimerHandler );
    } else {
        // we didn't call the BIOS handler so acknowledge the interrupt
        outp( 0x20, 0x20 );
    }
}
*/