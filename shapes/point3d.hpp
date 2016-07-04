#ifndef POINT3D_HPP
#define POINT3D_HPP

class Point3D {
    int x, y, z;
    
    public:
        Point3D();
        Point3D(int, int, int);

        int getX() {return x;};
        int getY() {return y;};
        int getZ() {return z;};
};

typedef Point3D Vector3i;

#endif