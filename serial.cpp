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
}

bool Serial::isTransmitEmpty() {
    return (inp(LSR) & 0x20);
}

void Serial::sendChar(unsigned char data){
    if(baseIO == -1) return;
    
    while(!isTransmitEmpty()){
        //spin until we can send...
    }
    outp(baseIO, data);
    delay(1);
};

void Serial::sendString(std::string str){
    for(int i=0;i<str.length();i++){
        sendChar(str.c_str()[i]);
    }
}