#include "rasterize.hpp"

#define PIXEL_OFFSET(X, Y) VGA_Y_OFFSETS[Y] + X

template <typename T>
T lerp(T x1, T x2, float weight){
    assert(weight >= 0.0f && weight <= 1.0f);
    return x1 + (x2 - x1) * weight;
}

void swap(int &a, int &b){
    int c = a;
    a = b;
    b = c;
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

void draw_top_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color){
    int triangle_height = screenPoints[2].y - screenPoints[0].y;
    int scanlines = 0;

    int v_top    = triangle.getTexturePoint(0).y * 256;
    int v_bottom = triangle.getTexturePoint(1).y * 256;
    int v_steps  = v_bottom - v_top;

/*
    if(v_steps == 0){
        _setvideomode(_DEFAULTMODE);
        printf("triangle top texture height is 0!\n");
        printf("v_top %d, v_bottom %d\n", v_top, v_bottom);
        printf("v0 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(0).x, triangle.getTexturePoint(0).y, triangle.getTexturePoint(0).z);
        printf("v1 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(1).x, triangle.getTexturePoint(1).y, triangle.getTexturePoint(1).z);
        printf("v2 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(2).x, triangle.getTexturePoint(2).y, triangle.getTexturePoint(2).z);
        exit(0);
    }
*/

    for(int y = screenPoints[0].y; y <= screenPoints[1].y; y++){
        if(y < 0) {
            continue;
        }
        //printf("lerp is %d. parameters are %d, %d, %d, %d\n", v, v_top, v_bottom, scanlines, screenPoints[2].y);
        int top_segment_height = screenPoints[1].y - screenPoints[0].y + 1;
        int v = (int)lerp(v_top, v_bottom, ((float)scanlines / (float)top_segment_height));

        if(triangle_height == 0){
            break;
        }
        float alpha = (float)(y - screenPoints[0].y) / (float)triangle_height;
        assert(top_segment_height != 0);
        float beta  = (float)(y - screenPoints[0].y) / (float)top_segment_height;

        Point A = lerp(screenPoints[0], screenPoints[2], alpha);
        Point B = lerp(screenPoints[0], screenPoints[1], beta);

        if(A.x > B.x) std::swap(A, B);           
        for(int horiz=A.x; horiz<=B.x; horiz++){
            if(horiz >= 0 && horiz < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT){
                float z = triangle.solve_plane_for_z(Point(horiz, y));
                color = checkerboardTexture[0 + v*256];
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

void draw_bottom_triangle(unsigned char *pixels, float *zbuffer, Triangle triangle, Point *screenPoints, int color){
    int triangle_height = screenPoints[2].y - screenPoints[0].y;
    int scanlines = 0;

    int v_top    = triangle.getTexturePoint(0).y * 256;
    int v_bottom = triangle.getTexturePoint(2).y * 256;
    int v_steps  = v_bottom - v_top;

/*
    if(v_steps == 0){
        _setvideomode(_DEFAULTMODE);
        printf("triangle bottom texture height is 0!\n");
        printf("v_top %d, v_bottom %d\n", v_top, v_bottom);
        printf("v0 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(0).x, triangle.getTexturePoint(0).y, triangle.getTexturePoint(0).z);
        printf("v1 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(1).x, triangle.getTexturePoint(1).y, triangle.getTexturePoint(1).z);
        printf("v2 U,V,W: %f,%f,%f\n", triangle.getTexturePoint(2).x, triangle.getTexturePoint(2).y, triangle.getTexturePoint(2).z);
        exit(0);
    }
*/

    for(int y = screenPoints[1].y; y <= screenPoints[2].y; y++){
        if(y < 0){
            continue;
        }

        //printf("lerp is %d. parameters are %d, %d, %d, %d\n", v, v_top, v_bottom, scanlines, screenPoints[2].y);
        int bottom_segment_height = screenPoints[2].y - screenPoints[1].y + 1;
        int v = (int)lerp(v_top, v_bottom, ((float)scanlines / (float)bottom_segment_height));

        if(triangle_height == 0){
            break;
        }

        float alpha = (float)(y - screenPoints[0].y) / (float)triangle_height;
        assert(bottom_segment_height != 0);
        float beta  = (float)(y - screenPoints[1].y) / (float)bottom_segment_height;

        Point A = lerp(screenPoints[0], screenPoints[2], alpha);
        Point B = lerp(screenPoints[1], screenPoints[2], beta);

        if(A.x > B.x) std::swap(A, B);           
        for(int horiz=A.x; horiz<=B.x; horiz++){
            
            if((horiz >= 0 && horiz < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)){
                float z = triangle.solve_plane_for_z(Point(horiz, y));
                color = checkerboardTexture[0 + v*256];
                if(z < zbuffer[PIXEL_OFFSET(horiz, y)]){
                    assert(horiz <= 319 && y <= 199);
                    zbuffer[PIXEL_OFFSET(horiz, y)] = z;
                    setPixel(pixels, horiz, y, color);
                }
            }
        }

        scanlines++;
    }
}

void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, int color, bool filled){    
    if(checkerboardTexture == NULL){
        checkerboardTexture = (char*)malloc(256*256);
        int col0, col1;
        for(int row=0;row<256;row++){
            if(row % 2 == 0){
                col0 = 0x0F; //white
                col1 = 0x10; //black
            } else {
                col0 = 0x10; //black
                col1 = 0x0F; //white
            }
            for(int col=0;col<32;col++){
                checkerboardTexture[(row*256) + col] = col0;
            }
            for(int col=32;col<64;col++){
                checkerboardTexture[(row*256) + col] = col1;
            }
            for(int col=64;col<96;col++){
                checkerboardTexture[(row*256) + col] = col0;
            }
            for(int col=96;col<128;col++){
                checkerboardTexture[(row*256) + col] = col1;
            }
            for(int col=128;col<160;col++){
                checkerboardTexture[(row*256) + col] = col0;
            }
            for(int col=160;col<192;col++){
                checkerboardTexture[(row*256) + col] = col1;
            }
            for(int col=192;col<224;col++){
                checkerboardTexture[(row*256) + col] = col0;
            }
            for(int col=224;col<256;col++){
                checkerboardTexture[(row*256) + col] = col1;
            }
        }
    }

    /* Calculate the screen coordinates of the triangle. */
    Point screenPoints[3];
    for(int i=0;i<3;i++){
        screenPoints[i] = Point((int)triangle.getGeometryPoints()[i].x, (int)triangle.getGeometryPoints()[i].y);
    }
    
    //draw_line(pixels, screenPoints[0], screenPoints[1], COLOR_GREEN);
    //draw_line(pixels, screenPoints[1], screenPoints[2], COLOR_GREEN);
    //draw_line(pixels, screenPoints[2], screenPoints[0], COLOR_RED);

    draw_top_triangle(pixels, zbuffer, triangle, screenPoints, color);
    draw_bottom_triangle(pixels, zbuffer, triangle, screenPoints, color);
}

