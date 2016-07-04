#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "point.hpp"
#include "vector3f.hpp"

class Triangle {
    Vector3f points[3];

    public:
    Triangle(Vector3f a, Vector3f b, Vector3f c) {
        points[0] = Vector3f(a.x, a.y, a.z);
        points[1] = Vector3f(b.x, b.y, b.z);
        points[2] = Vector3f(c.x, c.y, c.z);
    }
    
    Vector3f* getPoints() { return points; };
    Vector3f getPoint(int point) { return points[point]; };

    Vector3f barycentric_point(Point P){
        
        Vector3f U = Vector3f::cross_product(
            Vector3f(points[2].x - points[0].x,
                     points[1].x - points[0].x,
                     points[0].x - P.getX()),
            Vector3f(points[2].y - points[0].y,
                     points[1].y - points[0].y,
                     points[0].y - P.getX()));

        if(std::abs(U.y) < 1) return Vector3f(-1, 1, 1); //triangle is degenerate

        return Vector3f(1.0 - (U.x + U.y)/U.z, U.y/U.z, U.x/U.z);
    }
};

#endif