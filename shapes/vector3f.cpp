#include "vector3f.hpp"

Vector3f Vector3f::rotateAroundXAxis(float rotationDegrees){
    float sin = std::sin(DEG_TO_RAD(rotationDegrees));
    float cos = std::cos(DEG_TO_RAD(rotationDegrees));

    return Vector3f((x),
                    (y * cos) - (z * sin),
                    (y * sin) + (z * cos));
}

Vector3f Vector3f::rotateAroundYAxis(float rotationDegrees){
    float sin = std::sin(DEG_TO_RAD(rotationDegrees));
    float cos = std::cos(DEG_TO_RAD(rotationDegrees));

    return Vector3f(x * cos + z * sin,
                    y,
                    -x * sin + z * cos);
}

Vector3f Vector3f::rotateAroundZAxis(float rotationDegrees){
    float sin = std::sin(DEG_TO_RAD(rotationDegrees));
    float cos = std::cos(DEG_TO_RAD(rotationDegrees));

    return Vector3f(x * cos - y * sin,
                    x * sin + y * cos,
                    z);
}