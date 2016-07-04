#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <string>
#include <conio.h>
#include <i86.h>

enum SERIAL_SPEED { SER_9600_BAUD, SER_115200_BAUD };

class Serial {    
    int baseIO; //0x3F8

    //registers
    int IER; //0x3F9
    int IIR; //0x3FA
    int LCR; //0x3FB
    int MCR; //0x3FC
    int LSR; //0x3FD
    int MSR; //0x3FE
    int SR;  //0x3FF

    bool isTransmitEmpty();

    public:        
    Serial(){ baseIO = -1; };
    Serial(int _base, SERIAL_SPEED _baudRate);
    void sendChar(unsigned char data);
    void sendString(std::string str);
};

Serial serialPort;

#endif