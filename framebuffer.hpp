#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <math.h>

#include "includes\defines.hpp"
#include "fonts\font.hpp"
#include "shapes\shapes.hpp"
#include "shapes\triangle.hpp"
#include "includes\wavefront.hpp"
#include "includes\matrix.hpp"
#include "rasterize.hpp"

typedef unsigned char Pixel;

class Framebuffer {
    float *zbuffer;
    
    Pixel *pixels;
    unsigned short selector;

    public:
    void draw_line(Point, Point, int);
    void draw_polygon(Point points[], int num_points, Point origin, int rotation_angle, int color);
    void draw_filled_polygon(Point points[], int num_points, Point origin, int rotation_angle, int color);

    void reset_zbuffer();

    void draw_triangle(Triangle, Point, int);
    //void draw_filled_triangle(Triangle, Vector3f[], int);

    bool draw_face(WavefrontObject model, Vector3f eye, Vector3f center, int face);
    
    void draw_rectangle(Point, int, int, int, int);
    void draw_rectangle_filled(Point, int, int, int);
    void draw_area(const Pixel *source, Point start, Size2D size);
    void clear_area(Point start, Size2D size);
    void setPixel(Point, int);
    void setPixel(int, int, int);
    void overlay(Framebuffer source, int size);
    Pixel *getPixels();

    void putString(const char *str, int len, Point destination, int vga_color, Font font);
    void putString(std::string str, Point destination, int color, Font font);
    void putGlyph(Pixel *tile, int sizeX, int sizeY, int destX, int destY, int vga_color);

    Framebuffer();
};

//drawing routines
double getSlope(Point, Point);
double getYIntercept(Point, double);

Point offset(Point local, Point origin){
    Point point = Point(local.getX() + origin.getX(), local.getY() + origin.getY());
    return point;
}

Vector3f vector_offset(Vector3f local, Point origin){
    Vector3f vprime = Vector3f(local.x + origin.getX(), local.y + origin.getY(), local.z);
    return vprime;
}

#endif
