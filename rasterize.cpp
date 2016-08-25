#include "rasterize.hpp"

#define PIXEL_OFFSET(X, Y) VGA_Y_OFFSETS[Y] + X

void swap(int &a, int &b){
    int c = a;
    a = b;
    b = c;
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

    return Vector3f(-1.0, 1.0, 1.0);
}

void draw_line(unsigned char* pixels, Point start, Point end, int color){
    //Draw a 1-pixel thick line between two points.
    int x1 = start.getX();
    int x2 = end.getX();
    int y1 = start.getY();
    int y2 = end.getY();

    //Bresenham's algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
        swap(x1, y1);
        swap(x2, y2);
    }

    if(x1 > x2){
        swap(x1, x2);
        swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {
            if(BETWEEN(x, 0, SCREEN_HEIGHT) && BETWEEN(y, 0, SCREEN_WIDTH)){
                setPixel(pixels, y, x, color);
            }
        }
        else
        {
            if(BETWEEN(x, 0, SCREEN_WIDTH) && BETWEEN(y, 0, SCREEN_HEIGHT)){
                setPixel(pixels, x, y, color);
            }
        }
 
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }

    }
}

void draw_bottom_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color){
    int triangle_height = screenPoints[2].y - screenPoints[0].y;
    int scanlines = 0;

    int v_top    = triangle.getTexturePoint(0).y * 256;
    int v_bottom = triangle.getTexturePoint(1).y * 256;
    int v_steps  = v_bottom - v_top;

    for(int y = screenPoints[0].y; y <= screenPoints[1].y; y++){
        if(y < 0) {
            continue;
        }
        //printf("lerp is %d. parameters are %d, %d, %d, %d\n", v, v_top, v_bottom, scanlines, screenPoints[2].y);
        int top_segment_height = screenPoints[1].y - screenPoints[0].y;
        //int v = (int)lerp(v_top, v_bottom, ((float)scanlines / (float)top_segment_height));
        int v = 0;
        int u = 0;

        if(triangle_height == 0){
            break;
        }
        float alpha, beta;
        
        //assert(top_segment_height != 0);

        alpha = (float)(y - screenPoints[0].y) / (float)triangle_height;
        if(top_segment_height == 0){
            beta = 0;
        } else {
            beta = (float)(y - screenPoints[0].y) / (float)top_segment_height;
        }

        Point A = lerp(screenPoints[0], screenPoints[2], alpha);
        Point B = lerp(screenPoints[0], screenPoints[1], beta);

        if(A.x > B.x) std::swap(A, B);           
        for(int horiz=A.x; horiz<=B.x; horiz++){
            if(horiz >= 0 && horiz < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT){
                //Vector3f barycentric = barycentric_point(triangle.geometryPoints[0], triangle.geometryPoints[1], triangle.geometryPoints[2], Point(horiz, y));
                float z = triangle.solve_plane_for_z(Point(horiz, y));

                //texture: X = U, Y = V, Z = W.
                //W is always 1 here so simplify (triangle.texturePoints[0].x / triangle.texturePoints[0].z) to triangle.texturePoints[0].x
                //int u_prime = (int)(((barycentric.x * triangle.texturePoints[0].x + barycentric.y * triangle.texturePoints[1].x + barycentric.z * triangle.texturePoints[2].x)) * 256);
                //int v_prime = (int)(((barycentric.x * triangle.texturePoints[0].y + barycentric.y * triangle.texturePoints[1].y + barycentric.z * triangle.texturePoints[2].y)) * 256);
                //color = checkerboardTexture[u_prime + (v_prime*256)];

                if(z < zbuffer[PIXEL_OFFSET(horiz, y)]){
                    zbuffer[PIXEL_OFFSET(horiz, y)] = z;
                    assert(horiz <= 319 && y <= 199);
                    setPixel(pixels, horiz, y, color);
                }
            }
        }
        scanlines++;
    }    
}

void draw_top_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color){
    int triangle_height = screenPoints[2].y - screenPoints[0].y;
    float uv_triangle_height = triangle.getTexturePoint(2).y - triangle.getTexturePoint(0).y;

    int v_top    = triangle.getTexturePoint(0).y * 256;
    int v_bottom = triangle.getTexturePoint(2).y * 256;
    int v_steps  = v_bottom - v_top;

    float u_left    = triangle.getTexturePoint(0).x;
    float u_right   = triangle.getTexturePoint(2).x;
    float u_slope = (float)(v_bottom - v_top) / (float)(u_right - u_left);
    float current_right = u_left;

    for(int screen_y = screenPoints[1].y; screen_y <= screenPoints[2].y; screen_y++){
        if(screen_y < 0){
            continue;
        }

        float how_far_down_ratio = screen_y / screenPoints[2].y;

        //printf("lerp is %d. parameters are %d, %d, %d, %d\n", v, v_top, v_bottom, scanlines, screenPoints[2].y);
        int bottom_segment_height = screenPoints[2].y - screenPoints[1].y;

        if(triangle_height == 0){
            break;
        }

        float alpha, beta;

        alpha = (float)(screen_y - screenPoints[0].y) / (float)triangle_height;
        if(bottom_segment_height == 0){
            beta = 0.0;
        } else {
            beta = (float)(screen_y - screenPoints[1].y) / (float)bottom_segment_height;
        }

        Point A = lerp(screenPoints[0], screenPoints[2], alpha);
        Point B = lerp(screenPoints[1], screenPoints[2], beta);

        //find UV alpha and beta slope angles
        //int v = (int)lerp(v_top, v_bottom, how_far_down_ratio);
        int width = A.x + B.x;

        if(A.x > B.x) std::swap(A, B);           
        for(int screen_x=A.x; screen_x<=B.x; screen_x++){
            
            if((screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)){
                float z = triangle.solve_plane_for_z(Point(screen_x, screen_y));

                //texture: X = U, Y = V, Z = W.
                //W is always 1 here so simplify (triangle.texturePoints[0].x / triangle.texturePoints[0].z) to triangle.texturePoints[0].x? wrong
                //don't use the screen points to calculate barycentric coordinates, use clip coordinates
                //Vector3f barycentric = barycentric_point(triangle.geometryPoints[0], triangle.geometryPoints[1], triangle.geometryPoints[2], Point(screen_x, screen_y));
                //int u_prime = (int)((barycentric.x * triangle.texturePoints[0].x + barycentric.y * triangle.texturePoints[1].x + barycentric.z * triangle.texturePoints[2].x) * 256);
                //int v_prime = (int)((barycentric.x * triangle.texturePoints[0].y + barycentric.y * triangle.texturePoints[1].y + barycentric.z * triangle.texturePoints[2].y) * 256);
                //color = checkerboardTexture[u_prime + (v_prime*256)];

                if(z < zbuffer[PIXEL_OFFSET(screen_x, screen_y)]){
                    assert(screen_x <= 319 && screen_y <= 199);
                    zbuffer[PIXEL_OFFSET(screen_x, screen_y)] = z;
                    setPixel(pixels, screen_x, screen_y, color);

#ifdef PIXEL_DEBUGGING
                    memcpy(VGA_PTR, pixels, 320*200);
                    printf("Barycentric: %f %f %f\n X %f %f %f\n Y %f %f %f\n Z %f %f %f\n UV: %f %f\n",
                        barycentric.x,
                        barycentric.y,
                        barycentric.z,
                        triangle.texturePoints[0].x,
                        triangle.texturePoints[0].y,
                        triangle.texturePoints[0].z,
                        triangle.texturePoints[1].x,
                        triangle.texturePoints[1].y,
                        triangle.texturePoints[1].z,
                        triangle.texturePoints[2].x,
                        triangle.texturePoints[2].y,
                        triangle.texturePoints[2].z,
                        barycentric.x * triangle.texturePoints[0].x + barycentric.y * triangle.texturePoints[1].x + barycentric.z * triangle.texturePoints[2].x,
                        barycentric.x * triangle.texturePoints[0].y + barycentric.y * triangle.texturePoints[1].y + barycentric.z * triangle.texturePoints[2].y);
                    delay(1);
#endif

                }
            }
        }

    }
}

void construct_checkerboard(){
    checkerboardTexture = (char*)malloc(256*256);
    int col0, col1;
    bool switched = false;

    for(int row=0;row<256;row++){
        if(row % 32 == 0){
            if(switched){
                col0 = 0x10; //black
                col1 = 0x0F; //white
                switched = false;
            } else {
                col0 = 0x0F; //white
                col1 = 0x10; //black
                switched = true;
            }
        }

        for(int col=0;col<32;col++){
            checkerboardTexture[(row*256) + col] = col0;
            continue;
        }
        for(int col=32;col<64;col++){
            checkerboardTexture[(row*256) + col] = col1;
            continue;
        }
        for(int col=64;col<96;col++){
            checkerboardTexture[(row*256) + col] = col0;
            continue;
        }
        for(int col=96;col<128;col++){
            checkerboardTexture[(row*256) + col] = col1;
            continue;
        }
        for(int col=128;col<160;col++){
            checkerboardTexture[(row*256) + col] = col0;
            continue;
        }
        for(int col=160;col<192;col++){
            checkerboardTexture[(row*256) + col] = col1;
            continue;
        }
        for(int col=192;col<224;col++){
            checkerboardTexture[(row*256) + col] = col0;
            continue;
        }
        for(int col=224;col<256;col++){
            checkerboardTexture[(row*256) + col] = col1;
            continue;
        }
    }
}

void scanline_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color){
    int boundingBoxX[2] = {321, -1};
	int boundingBoxY[2] = {201, -1};

    for(int i=0; i<3; i++){
        if(screenPoints[i].x < boundingBoxX[0]) boundingBoxX[0] = screenPoints[i].x;
        if(screenPoints[i].x > boundingBoxX[1]) boundingBoxX[1] = screenPoints[i].x;

        if(screenPoints[i].y < boundingBoxY[0]) boundingBoxY[0] = screenPoints[i].y;
        if(screenPoints[i].y > boundingBoxY[1]) boundingBoxY[1] = screenPoints[i].y;
    }

    for(int scanline = std::max(boundingBoxY[0], 0); scanline <= std::min(SCREEN_HEIGHT-1, boundingBoxY[1]); scanline++){
        for(int x=std::max(0, boundingBoxX[0]);x<=std::min(SCREEN_WIDTH-1, boundingBoxX[1]);x++){
            //Is the point inside the triangle?      
            Vector3f barycentric = barycentric_point(screenPoints[0], screenPoints[1], screenPoints[2], Point(x, scanline));
            if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;
            
            //Yes, so get the point's Z.
            float z = 0;
            z += (triangle.geometryPoints[0].z * barycentric.x);
            z += (triangle.geometryPoints[1].z * barycentric.y);
            z += (triangle.geometryPoints[2].z * barycentric.z);

            //texture: X = U, Y = V, Z = W.
            //W is always 1 here so simplify (triangle.texturePoints[0].x / triangle.texturePoints[0].z) to triangle.texturePoints[0].x
            float w_prime = barycentric.x + barycentric.y + barycentric.z;
            int u_prime = (int)(((barycentric.x * triangle.texturePoints[0].x + barycentric.y * triangle.texturePoints[1].x + barycentric.z * triangle.texturePoints[2].x) / w_prime) * 256);
            int v_prime = (int)(((barycentric.x * triangle.texturePoints[0].y + barycentric.y * triangle.texturePoints[1].y + barycentric.z * triangle.texturePoints[2].y) / w_prime) * 256);
            color = checkerboardTexture[u_prime + (v_prime*256)];

            if(z < zbuffer[x + (scanline * VGA_WIDTH)]) {
                zbuffer[x + (scanline * VGA_WIDTH)] = z;
                setPixel(pixels, x, scanline, color);
            }
        }
    }
}

void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, int color, bool filled){    
    if(checkerboardTexture == NULL){
        construct_checkerboard();
    }

    /* Calculate the screen coordinates of the triangle. */
    Point screenPoints[3];
    for(int i=0;i<3;i++){
        screenPoints[i] = Point((int)(triangle.getGeometryPoints()[i].x+.5), (int)(triangle.getGeometryPoints()[i].y+.5));
    }
    
    //draw_line(pixels, screenPoints[0], screenPoints[1], COLOR_GREEN);
    //draw_line(pixels, screenPoints[1], screenPoints[2], COLOR_GREEN);
    //draw_line(pixels, screenPoints[2], screenPoints[0], COLOR_RED);

    draw_top_triangle(pixels, zbuffer, triangle, screenPoints, color);
    draw_bottom_triangle(pixels, zbuffer, triangle, screenPoints, color);
    //scanline_triangle(pixels, zbuffer, triangle, screenPoints, color);
}

