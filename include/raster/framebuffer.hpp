#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <math.h>

#include "defines.hpp"
#include "fonts\font.hpp"
#include "shapes\shapes.hpp"
#include "shapes\triangle.hpp"
#include "shapes\wavefront.hpp"
#include "objects\sceneobject.hpp"
#include "rasterize.hpp"

typedef enum blit_operation_t {
    BLIT_OVERWRITE_D
} BLIT_Operation;

typedef unsigned char Pixel;

class Framebuffer {
    float *zbuffer;
    
    unsigned short selector;

    public:
    void draw_line(Point, Point, int);
    void draw_polygon(Point points[], int num_points, Point origin, int rotation_angle, COLOR color);
    void draw_filled_polygon(Point points[], int num_points, Point origin, int rotation_angle, COLOR color);

    void reset_zbuffer();

    bool draw_face(SceneObject obj, Vector3f eye, Vector3f center, int face);
    
    void draw_rectangle(Point2D origin, Size2D size, int thickness, COLOR color);
    void draw_rectangle_filled(Point origin, Size2D size, COLOR color);
    void shade_rectangle(Point2D position, Size2D size, COLOR light, COLOR dark);

    void blit_area(const Pixel *source, Point start, Size2D size, BLIT_Operation blit_operation);
    void clear_area(Point start, Size2D size);
    void setPixel(Point, int);
    void setPixel(int, int, int);
    void overlay(Framebuffer source, int size);
    Pixel *pixels;

    void putString(const char *str, int len, Point destination, int vga_color, Font font);
    void putString(std::string str, Point destination, COLOR color, Font font);
    void putGlyph(Pixel *tile, Size2D size, Point2D destination, int vga_color);

    Framebuffer();
};

//drawing routines
double getSlope(Point, Point);
double getYIntercept(Point, double);

Vector3f applyTransformations(Vector3f v, Vector3f eye, Vector3f cameraRotation, Vector3f translation, Vector3f rotation, Vector3f scale);
Vector3f calculateSurfaceNormal(Vector3f *triangle);

#endif
