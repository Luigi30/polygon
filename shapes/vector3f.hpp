#ifndef VECTOR3F_HPP
#define VECTOR3F_HPP

#include <cmath>
#include <cstdio>

#include "point.hpp"

#define M_PI 3.141592653589793238462643383279502884
#define DEG_TO_RAD(X) ((X * M_PI) / 180.0)

class Vector3f {
    public:
        float x;
        float y;
        float z;

        Vector3f() { x = 0; y = 0; z = 0; };
        Vector3f(float _x, float _y, float _z) {
            x = _x;
            y = _y;
            z = _z;
        }

        Vector3f(const Vector3f &original){
            x = original.x;
            y = original.y;
            z = original.z;
        }

        Vector3f operator+(const Vector3f &second){
            return Vector3f(this->x + second.x, this->y + second.y, this->z + second.z);
        }

        Vector3f operator-(const Vector3f &second){
            return Vector3f(this->x - second.x, this->y - second.y, this->z - second.z);
        }

        float operator*(const Vector3f &second){
            return x*second.x + y*second.y + z*second.z;
        }

        Vector3f operator*(const float scalar){
            return Vector3f(x*scalar, y*scalar, z*scalar);
        }

        float length() {
            return std::sqrt(x*x + y*y + z*z);
        }

        static float dot_product(Vector3f a, Vector3f b){
            return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
        }

        Vector3f negate(){
            return Vector3f(-x, -y, -z);
        }

        operator Point2D() {
            int x_i, y_i;
            
            if(x > 0)
                x_i = (int)(x + 0.5);
            else
                x_i = (int)(x - 0.5);

            if(y > 0)
                y_i = (int)(y + 0.5);
            else
                y_i = (int)(y - 0.5);

            return Point2D(x_i, y_i);
        }

        Vector3f normalize(){
            float magnitude = length();

            if(magnitude != 0){
                return Vector3f(x/magnitude, y/magnitude, z/magnitude);
            } else {
                return Vector3f();
            }
        }

        static Vector3f cross_product(const Vector3f a, const Vector3f b){
            return Vector3f((a.y * b.z) - (a.z * b.y),
                            (a.z * b.x) - (a.x * b.z),
                            (a.x * b.y) - (a.y * b.x));
        }

        float carmack(float input){
            //Fast inverse floating point

            long i;
            float x2, y;
            const float threehalves = 1.5f;

            x2 = input * 0.5f;
            y = input;
            i = *(long*)&y;
            i = 0x5f3759df - (i >> 1); //what the fuck?
            y = *(float*) &i;
            y = y * (threehalves - (x2*y*y));
            return y;
        }

        Vector3f rotateAroundXAxis(float rotationDegrees);
        Vector3f rotateAroundYAxis(float rotationDegrees);
        Vector3f rotateAroundZAxis(float rotationDegrees);
};

#endif