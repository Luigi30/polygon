#ifndef RASTERIZE_HPP
#define RASTERIZE_HPP

#include "includes\matrix.hpp"
#include "includes\wavefront.hpp"

Vector3f eye(0,0,5);    //the camera coordinates
Vector3f center(0,0,0); //the camera is pointed at this point
                        //need to find rotation in order to rotate the camera
Vector3f up(0.0, 1.0, 0.0); //up

Matrix look_at(Vector3f vec_eye, Vector3f vec_center, Vector3f vec_up);
Matrix setup_viewport(int top, int bottom, int left, int right);
//Matrix perspective_projection(int top, int bottom, int left, int right, int near, int far);

/* Transformation matrix */
Matrix Viewport = setup_viewport(0, 200, 0, 320);
Matrix Projection = Matrix::identity(4);
Matrix View = look_at(eye, center, up);

void recalculate_transformation();

Matrix Transformation = Matrix::identity(4);

#endif