#include "triangle.hpp"

Triangle Triangle::sortByY(){
        int order[3]; //integer order of vertex Y coordinates
        order[0] = 0;
        order[1] = 1;
        order[2] = 2;

        bool sorted;

        do {
            sorted = true;
            
            if(geometryPoints[order[0]].y > geometryPoints[order[1]].y){
                std::swap(order[0], order[1]);
                sorted = false;
            }
            if(geometryPoints[order[0]].y > geometryPoints[order[2]].y){
                std::swap(order[0], order[2]);
                sorted = false;
            }
            if(geometryPoints[order[1]].y > geometryPoints[order[2]].y){
                std::swap(order[1], order[2]);
                sorted = false;
            }

            if((geometryPoints[order[0]].y == geometryPoints[order[1]].y) && (geometryPoints[order[0]].x > geometryPoints[order[1]].x)){
                std::swap(order[0], order[1]);
                sorted = false;
            }
            if((geometryPoints[order[0]].y == geometryPoints[order[2]].y) && (geometryPoints[order[0]].x > geometryPoints[order[2]].x)){
                std::swap(order[0], order[1]);
                sorted = false;
            }
            if((geometryPoints[order[1]].y == geometryPoints[order[2]].y) && (geometryPoints[order[1]].x > geometryPoints[order[2]].x)){
                std::swap(order[1], order[2]);
                sorted = false;
            }

        } while(!sorted);

        return Triangle(geometryPoints[order[0]], geometryPoints[order[1]], geometryPoints[order[2]], texturePoints[order[0]], texturePoints[order[1]], texturePoints[order[2]], clipPoints[order[0]], clipPoints[order[1]], clipPoints[order[2]]);
    }

float Triangle::solve_plane_for_z(Point P){
    //Solve the equation of a plane T for Z at point P.
    Vector3f A = geometryPoints[0];
    Vector3f B = geometryPoints[1];
    Vector3f C = geometryPoints[2];

    return A.z + (((((B.x-A.x)*(C.z-A.z)) - ((C.x-A.x)*(B.z-A.z))) / ((B.x-A.x)*(C.y-A.y) - (C.x-A.x)*(B.y-A.y))) * (P.y-A.y)) - ((((B.y-A.y)*(C.z-A.z) - (C.y-A.y)*(B.z-A.z)) / ((B.x-A.x)*(C.y-A.y) - (C.x-A.x)*(B.y-A.y))) * (P.x-A.x));
}