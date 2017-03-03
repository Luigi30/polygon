#ifndef POINT_HPP
#define POINT_HPP

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graph.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>

class Point {
    public:
        int x;
        int y;
    
        Point();
        Point(int, int);
        Point(char*);
        const int getX() { return x; };
        const int getY() { return y; };
        Point operator+(const Point& two){
            Point point = Point(this->x + two.x, this->y + two.y);
            return point;
        }
        Point operator-(const Point& two){
            Point point = Point(this->x - two.x, this->y - two.y);
            return point;
        }
        Point operator*(const int multiplier){
            Point point = Point(this->x * multiplier, this->y * multiplier);
            return point;
        }

        Point operator*(const float multiplier){
            Point point = Point(this->x * multiplier, this->y * multiplier);
            return point;
        }

        Point offset(Point offset_by){
            Point point = Point(x + offset_by.getX(), y + offset_by.getY());
            return point;
        }
};

typedef Point Vector2i;
typedef Point Point2D;
typedef Point Size2D;

#endif

