#include "triangle.hpp"

Triangle Triangle::sortByY(){
        int order[3]; //integer order of vertex Y coordinates
        int temp;
        order[0] = 0;
        order[1] = 1;
        order[2] = 2;

        bool sorted;

        do {
            sorted = true;
            
            if(points[order[0]].y > points[order[1]].y){
                temp = order[0];
                order[0] = order[1];
                order[1] = temp;
                sorted = false;
            }
            if(points[order[0]].y > points[order[2]].y){
                temp = order[0];
                order[0] = order[2];
                order[2] = temp;
                sorted = false;
            }
            if(points[order[1]].y > points[order[2]].y){
                temp = order[1];
                order[1] = order[2];
                order[2] = temp;
                sorted = false;
            }          
        } while(!sorted);

        return Triangle(points[order[0]], points[order[1]], points[order[2]]);
    }

float Triangle::solve_plane_for_z(Point P){
    //Solve the equation of a plane T for Z at point P.
    Vector3f A = points[0];
    Vector3f B = points[1];
    Vector3f C = points[2];

    return A.z + (((((B.x-A.x)*(C.z-A.z)) - ((C.x-A.x)*(B.z-A.z))) / ((B.x-A.x)*(C.y-A.y) - (C.x-A.x)*(B.y-A.y))) * (P.y-A.y)) - ((((B.y-A.y)*(C.z-A.z) - (C.y-A.y)*(B.z-A.z)) / ((B.x-A.x)*(C.y-A.y) - (C.x-A.x)*(B.y-A.y))) * (P.x-A.x));
}