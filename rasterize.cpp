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

Vector3f barycentric_point(Point A, Point B, Point C, Point P){
    /* Find the barycentric vector of triangle (A,B,C) and point P. */

    Vector3f s[2];

    s[1].x = C.y - A.y;
    s[1].y = B.y - A.y;
    s[1].z = A.y - P.y;

    s[0].x = C.x - A.x;
    s[0].y = B.x - A.x;
    s[0].z = A.x - P.x;

    Vector3f U = Vector3f::cross_product(s[0], s[1]);

    if(std::abs(U.z) > 1e-2){
        return Vector3f(1.0 - (U.x + U.y)/U.z, U.y/U.z, U.x/U.z);
    }

    //assert(false);
    return Vector3f(-1.0, 1.0, 1000.0);
    
}

void setPixel(unsigned char* pixels, int x, int y, int color){
    unsigned int coord = VGA_Y_OFFSETS[y] + x;
    pixels[coord] = color;
}

#define BETWEEN(token, A, B) (token >= A && token <= B)
void swap(int &a, int &b){
    int c = a;
    a = b;
    b = c;
}

void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, Vector3f *worldCoordinates, int color, bool filled){
    /* Calculate the screen coordinates of the triangle. */
    Point screenPoints[3];
    for(int i=0;i<3;i++){
        screenPoints[i] = Point((int)triangle.getPoints()[i].x + .5, (int)triangle.getPoints()[i].y + .5);
    }

    /* Find the bounding box of the triangle. */
    int boundingBoxX[2] = {321, -1};
    int boundingBoxY[2] = {201, -1};
    
    for(int i=0; i<3; i++){
        if(screenPoints[i].x < boundingBoxX[0]) boundingBoxX[0] = screenPoints[i].x;
        if(screenPoints[i].x > boundingBoxX[1]) boundingBoxX[1] = screenPoints[i].x;

        if(screenPoints[i].y < boundingBoxY[0]) boundingBoxY[0] = screenPoints[i].y;
        if(screenPoints[i].y > boundingBoxY[1]) boundingBoxY[1] = screenPoints[i].y;
    }

    if(filled){

        for(int scanline = std::max(boundingBoxY[0], 0); scanline <= std::min(SCREEN_HEIGHT-1, boundingBoxY[1]); scanline++){
            for(int x=std::max(0, boundingBoxX[0]);x<=std::min(SCREEN_WIDTH-1, boundingBoxX[1]);x++){

                /* Is the point inside the triangle? */      
                Vector3f barycentric = barycentric_point(screenPoints[0], screenPoints[1], screenPoints[2], Point(x, scanline));
                if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue; //If not, go to the next point

                /* Yes, so get the point's Z. */
                float z = 0;
                z += (worldCoordinates[0].z * barycentric.x);
                z += (worldCoordinates[1].z * barycentric.y);
                z += (worldCoordinates[2].z * barycentric.z);
            
                /* Check the Z-buffer. Should we draw this point? */
                unsigned int pixelIndex = x + VGA_Y_OFFSETS[scanline];
                if(z < zbuffer[pixelIndex]) {

                    /* Did we fuck up at some point? */
                    //assert(scanline >= 0);
                    //assert(x >= 0);
                    //assert(scanline < SCREEN_HEIGHT);
                    //assert(x < SCREEN_WIDTH);

                    /* Yes, draw the point. */
                    zbuffer[pixelIndex] = z;
                    pixels[pixelIndex] = color;
                }
            }
        }
    } else {
        for(int i=0;i<3;i++){
            unsigned int pixelIndex = screenPoints[i].x + VGA_Y_OFFSETS[screenPoints[i].y];            
            Vector3f barycentric = barycentric_point(screenPoints[0], screenPoints[1], screenPoints[2], screenPoints[i]);
                if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

            /* Yes, so get the point's Z. */
            float z = 0;
            z += (worldCoordinates[0].z * barycentric.x);
            z += (worldCoordinates[1].z * barycentric.y);
            z += (worldCoordinates[2].z * barycentric.z);

            if(z < zbuffer[pixelIndex]) {
                zbuffer[pixelIndex] = z;
                pixels[pixelIndex] = COLOR_GREEN;
            }
        }
    }

    /* Draw the triangle's wireframe. */
    /*
    for(int i=0;i<3;i++){
        Point start = triangle.getPoint(i);
        Point end = triangle.getPoint((i+1) % 3);
        draw_line(start, end, COLOR_GREEN);
    }
    */
}