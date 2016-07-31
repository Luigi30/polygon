#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "point.hpp"
#include "vector3f.hpp"

class Triangle {
    Vector3f geometryPoints[3];
    Vector3f texturePoints[3];

    public:
    Triangle(Vector3f a, Vector3f b, Vector3f c) {
        geometryPoints[0] = Vector3f(a.x, a.y, a.z);
        geometryPoints[1] = Vector3f(b.x, b.y, b.z);
        geometryPoints[2] = Vector3f(c.x, c.y, c.z);

        texturePoints[0] = Vector3f(0,0,0);
        texturePoints[1] = Vector3f(0,0,0);
        texturePoints[2] = Vector3f(0,0,0);
    }

    Triangle(Vector3f g_a, Vector3f g_b, Vector3f g_c, Vector3f t_a, Vector3f t_b, Vector3f t_c) {
        geometryPoints[0] = Vector3f(g_a.x, g_a.y, g_a.z);
        geometryPoints[1] = Vector3f(g_b.x, g_b.y, g_b.z);
        geometryPoints[2] = Vector3f(g_c.x, g_c.y, g_c.z);

        texturePoints[0] = Vector3f(t_a.x, t_a.y, t_a.z);
        texturePoints[1] = Vector3f(t_b.x, t_b.y, t_b.z);
        texturePoints[2] = Vector3f(t_c.x, t_c.y, t_c.z);
    }

    Vector3f* getGeometryPoints() { return geometryPoints; };
    Vector3f getGeometryPoint(int point) { return geometryPoints[point]; };

    Vector3f* getTexturePoints() { return texturePoints; };
    Vector3f getTexturePoint(int point) { return texturePoints[point]; };

    Triangle sortByY();
    float solve_plane_for_z(Point P);
};

#endif