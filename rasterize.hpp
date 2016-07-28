#ifndef RASTERIZE_HPP
#define RASTERIZE_HPP

#include <math.h>
#include <cassert>
#include <algorith>
#include "includes\defines.hpp"
#include "includes\wavefront.hpp"
#include "shapes\triangle.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VIEWPORT_WIDTH_3D 200
#define VIEWPORT_HEIGHT_3D 200
#define ZBUFFER_DEPTH 255

#define BETWEEN(token, A, B) (token >= A && token <= B)

Vector3f eye(0,0,-5);    //the camera coordinates
Vector3f center(0,0,0); //the camera is pointed at this point
Vector3f up(0.0, 1.0, 0.0); //up vector
Vector3f cameraRotation(0, 0, 0);

void draw_top_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color);
void draw_bottom_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color);

void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, int color, bool filled);

inline void setPixel(unsigned char* pixels, int x, int y, int color){
    pixels[VGA_Y_OFFSETS[y] + x] = color;
}

#endif