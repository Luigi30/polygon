#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "point.hpp"
#include "point3d.hpp"
#include "vector3f.hpp"

class Shape {
    char name[16];
    int numPoints;

    public:
    Point points[64];
    Shape() { strcpy(name, "NOSHAPE"); numPoints = 0; };
    Shape(char *_name, char *_rawPointText);
    int getNumPoints() { return numPoints; }
};

#endif
