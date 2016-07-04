#include <iterator>
#include <algorithm>
#include <stdio>

#include "font.hpp"

void fbPutGlyph(char *framebuffer, unsigned char *tile, Size2D size, Point destination, int vga_color){ //8x8 font tile   
    long bytePos = (destination.getY() * 320) + destination.getX();

    for(int y=0; y<size.getY(); y++) {
        //for each row of the tile
        if(tile[y] & 0x80) {
            framebuffer[bytePos] = vga_color;
        } else {
            //framebuffer[bytePos] = 0x00;
        }

        if(tile[y] & 0x40) {
            framebuffer[bytePos+1] = vga_color;
        } else {
            //framebuffer[bytePos+1] = 0x00;
        }

        if(tile[y] & 0x20) {
            framebuffer[bytePos+2] = vga_color;
        } else {
            //framebuffer[bytePos+2] = 0x00;
        }
        
        if(tile[y] & 0x10) {
            framebuffer[bytePos+3] = vga_color;
        } else {
            //framebuffer[bytePos+3] = 0x00;
        }
        
        if(tile[y] & 0x08) {
            framebuffer[bytePos+4] = vga_color;
        } else {
            //framebuffer[bytePos+4] = 0x00;
        }
        
        if(tile[y] & 0x04) {
            framebuffer[bytePos+5] = vga_color;
        } else {
            //framebuffer[bytePos+5] = 0x00;
        }
        
        if(tile[y] & 0x02) {
            framebuffer[bytePos+6] = vga_color;
        } else {
            //framebuffer[bytePos+6] = 0x00;
        }
        
        if(tile[y] & 0x01) {
            framebuffer[bytePos+7] = vga_color;
        } else {
            //framebuffer[bytePos+7] = 0x00;
        }
        
        bytePos += 320;
    }
    
};

//void putFontStringIntoFramebuffer(char *framebuffer, char *str, int len, int destX, int destY, int vga_color, unsigned char *font, int sizeX, int sizeY){
void fbPutString(char *framebuffer, const char *str, int len, Point destination, int vga_color, Font font){
    int sizeX = font.getSizeX();
    int sizeY = font.getSizeY();

    for(int i=0;i<len;i++){        
        fbPutGlyph(framebuffer, font.getGlyph(str[i]), Size2D(sizeX, sizeY), Point(destination.getX()+(sizeX * i), destination.getY()), vga_color);
    }
}

void fbPutString(char *framebuffer, std::string str, Point destination, int color, Font font){
    fbPutString(framebuffer, str.c_str(), str.length(), destination, color, font);
}
