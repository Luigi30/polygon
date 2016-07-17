#ifndef TIMER_HPP
#define TIMER_HPP

/* Timer class. One master clock interrupt is fired at 240Hz. */
enum TIMER_STATUS { TIMER_STOP, TIMER_START, TIMER_EXPIRED };

#define TIMER_MASTER 0
#define TIMER_MIDI   1

class Timer {
    volatile TIMER_STATUS status;
    int length; //the timer's length in master clock interrupts
    volatile int masterClockInterruptsUntilExpiration; //decremented each tick()
    
    public:
    Timer();
    Timer(int ticksLength);
    TIMER_STATUS getStatus();
    void start();
    void stop();
    
    void tick(); //fires on each timer interrupt
};

Timer timerList[8];

#endif TIMER_HPP