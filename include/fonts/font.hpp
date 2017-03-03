/* Font routines. */

#ifndef FONT_HPP
#define FONT_HPP

#include <cstddef>
#include <string>
#include <graph.h>
#include "..\shapes\shapes.hpp"
#include "font8x8.hpp"
#include "font4x6.hpp"
#include "font6x8.hpp"
#include "font5x8.hpp"

class Font {
    unsigned char *glyphs;
    int sizeX;
    int sizeY;

    public:
    unsigned char *getGlyph(int codepoint){
        unsigned char *retval = glyphs;
        retval = retval + (codepoint * sizeY);
        return retval;
    };
    int getSizeX() { return sizeX; };
    int getSizeY() { return sizeY; };
    
    Font() { glyphs = NULL; sizeX = 0; sizeY = 0; };
    
    Font(unsigned char* _fontData, int _sizeX, int _sizeY) {
        glyphs = _fontData;
        sizeX = _sizeX;
        sizeY = _sizeY;
    };
};

Font fontsList[16] = {  Font(console_font_8x8, 8, 8),
                        Font(console_font_4x6, 4, 6),
                        Font(console_font_6x8, 6, 8),
                        Font(console_font_5x8, 5, 8) };

#define FONT_8x8 fontsList[0]
#define FONT_4x6 fontsList[1]
#define FONT_6x8 fontsList[2]
#define FONT_5x8 fontsList[3]

void fbPutString(char *framebuffer, const char *str, int len, Point destination, int vga_color, Font font);
void fbPutString(char *framebuffer, std::string str, Point destination, int color, Font font);
void fbPutGlyph(char *framebuffer, unsigned char *tile, int sizeX, int sizeY, int destX, int destY, int vga_color);

#endif
