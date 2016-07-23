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

    Triangle sortByY();
    float solve_plane_for_z(Point P);
};

#endif