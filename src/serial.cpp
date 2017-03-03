#include "serial.hpp"

Serial::Serial(int _base, SERIAL_SPEED _baudRate){    
    baseIO = _base;

    //set up the baud rate
    outp(baseIO + 0x03, 0x80);
    switch(_baudRate){
        case SER_9600_BAUD:
            outp(baseIO + 0x00, 0x0C);
            outp(baseIO + 0x01, 0x00); //divisors set for 9600 baud
            break;
        case SER_115200_BAUD:
            outp(baseIO + 0x00, 0x01);
            outp(baseIO + 0x01, 0x00); //divisors set for 115200 baud
            break;
    }
    outp(baseIO + 0x03, 0x03); //8,N,1
    outp(baseIO + 0x02, 0xC7); //FIFO register
    outp(baseIO + 0x04, 0x0B); //DTR, RTS, OUT2
}

bool Serial::isTransmitEmpty() {
    return (inp(LSR) & 0x20);
}

bool Serial::ClearToSend(){
    return (inp(MSR) & 0x10);
}

void Serial::sendChar(unsigned char data){
    if(baseIO == -1) return;
    outp(baseIO, data);
    delay(1);
};

void Serial::sendString(std::string str){
    for(int i=0;i<str.length();i++){
        sendChar(str.c_str()[i]);
    }
}