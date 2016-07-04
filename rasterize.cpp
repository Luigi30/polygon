#include "rasterize.hpp"

Matrix look_at(Vector3f vec_eye, Vector3f vec_center, Vector3f vec_up){
    /*
    Vector3f z = (vec_eye-vec_center).normalize();
    Vector3f x = Vector3f::cross_product(vec_up, z).normalize();
    Vector3f y = Vector3f::cross_product(z, x).normalize();
    */

    Vector3f z = (vec_eye - vec_center);
    z = z.normalize();

    Vector3f y = vec_up;
    Vector3f x = Vector3f::cross_product(y, z);
    y = Vector3f::cross_product(z, x);

    x = x.normalize();
    y = y.normalize();
        
    Matrix result = Matrix::identity(4);

    result[0][0] = x.x;
    result[0][1] = x.y;
    result[0][2] = x.z;
    result[1][0] = y.x;
    result[1][1] = y.y;
    result[1][2] = y.z;
    result[2][0] = z.x;
    result[2][1] = z.y;
    result[2][2] = z.z;

    result[0][3] = (x*-1) * vec_eye;
    result[1][3] = (y*-1) * vec_eye;
    result[2][3] = (z*-1) * vec_eye;
    result[3][3] = 1.0f;
    
    return result;
}

Matrix setup_viewport(int top, int bottom, int left, int right){
    Matrix m = Matrix::identity(4);
    m[0][0] = (right - left) / 2;
    m[1][1] = (top - bottom) / 2;
    m[2][2] = 1.0/2.0;

    m[0][3] = (right + left) / 2;
    m[1][3] = (top + bottom) / 2;
    m[2][3] = 1.0/2.0;
    return m;
}

void recalculate_transformation(){
    View = look_at(eye, center, up);
    Transformation = Viewport * Projection * View;
}