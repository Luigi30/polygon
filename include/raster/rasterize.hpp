#ifndef RASTERIZE_HPP
#define RASTERIZE_HPP

#include <math.h>
#include <cassert>
#include <algorith>
#include "defines.hpp"
#include "shapes\triangle.hpp"
#include "shapes\wavefront.hpp"

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

void draw_top_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, COLOR color);
void draw_bottom_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, COLOR color);
void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, COLOR color);

inline void setPixel(unsigned char* pixels, int x, int y, COLOR color){
    pixels[VGA_Y_OFFSETS[y] + x] = color;
}

template <typename T>
T lerp(T x1, T x2, float weight){
    assert(weight >= 0.0f && weight <= 1.0f);
    return x1 + (x2 - x1) * weight;
}

#endif