#include "point.hpp"

Point::Point(){
    x = 0;
    y = 0;
}

Point::Point(int _x, int _y){
    x = _x;
    y = _y;
};

Point::Point(char *str) {       
    //A token in the format X,Y
    char buffer[16];
    strcpy(buffer, str);

    int len = strlen(str);
    char strX[4] = "";
    char strY[4] = "";

    char* ptr = (char*)memchr(str, ',', len);
    int afterComma = strlen(ptr);
    strcpy(strY, ptr+1);
    y = atol(strY);
    *ptr = '\0';
    strcpy(strX, str);
    x = atol(str);
}
