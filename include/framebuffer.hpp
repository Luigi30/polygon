#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <math.h>

#include "defines.hpp"
#include "fonts\font.hpp"
#include "shapes\shapes.hpp"
#include "shapes\triangle.hpp"
#include "wavefront.hpp"
#include "matrix.hpp"
#include "objects\sceneobject.hpp"
#include "rasterize.hpp"

typedef unsigned char Pixel;

class Framebuffer {
    float *zbuffer;
    
    unsigned short selector;

    public:
    void draw_line(Point, Point, int);
    void draw_polygon(Point points[], int num_points, Point origin, int rotation_angle, int color);
    void draw_filled_polygon(Point points[], int num_points, Point origin, int rotation_angle, int color);

    void reset_zbuffer();

    bool draw_face(SceneObject obj, Vector3f eye, Vector3f center, int face);
    
    void draw_rectangle(Point, int, int, int, int);
    void draw_rectangle_filled(Point, int, int, int);
    void draw_area(const Pixel *source, Point start, Size2D size);
    void clear_area(Point start, Size2D size);
    void setPixel(Point, int);
    void setPixel(int, int, int);
    void overlay(Framebuffer source, int size);
    Pixel *pixels;

    void putString(const char *str, int len, Point destination, int vga_color, Font font);
    void putString(std::string str, Point destination, int color, Font font);
    void putGlyph(Pixel *tile, int sizeX, int sizeY, int destX, int destY, int vga_color);

    Framebuffer();
};

//drawing routines
double getSlope(Point, Point);
double getYIntercept(Point, double);

Vector3f applyTransformations(Vector3f v, Vector3f eye, Vector3f cameraRotation, Vector3f translation, Vector3f rotation, Vector3f scale);
Vector3f calculateSurfaceNormal(Vector3f *triangle);

#endif
