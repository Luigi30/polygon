#include "timer.hpp"

Timer::Timer(int ticksLength){
    status = TIMER_STOP;
    length = ticksLength;
    masterClockInterruptsUntilExpiration = 0;
}

Timer::Timer(){
    status = TIMER_STOP;
    length = 0;
    masterClockInterruptsUntilExpiration = 0;
}

TIMER_STATUS Timer::getStatus(){
    return status;
}

void Timer::tick(){
    //The system timer has ticked.
    if(status == TIMER_START) {
        masterClockInterruptsUntilExpiration--;
        if(masterClockInterruptsUntilExpiration == 0) {
            status = TIMER_EXPIRED;
        }
    }
    //if timer is stopped or expired, do nothing
}

void Timer::start(){
    masterClockInterruptsUntilExpiration = length;
    status = TIMER_START;
}

void Timer::stop(){
    status = TIMER_STOP;
}