#include "rasterize.hpp"

#define PIXEL_OFFSET(X, Y) VGA_Y_OFFSETS[Y] + X

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

void draw_projected_triangle(unsigned char* pixels, float* zbuffer, Triangle triangle, int color, bool filled){    
    /* Calculate the screen coordinates of the triangle. */
    Point screenPoints[3];
    for(int i=0;i<3;i++){
        screenPoints[i] = Point((int)triangle.getPoints()[i].x + .5, (int)triangle.getPoints()[i].y + .5);
    }
    
    if(filled){
        //draw_line(pixels, screenPoints[0], screenPoints[1], COLOR_GREEN);
        //draw_line(pixels, screenPoints[1], screenPoints[2], COLOR_GREEN);
        //draw_line(pixels, screenPoints[2], screenPoints[0], COLOR_RED);

        int triangle_height = screenPoints[2].y - screenPoints[0].y;
        
        //split triangle in half along the middle point

        //draw top half
        for(int y = screenPoints[0].y; y <= screenPoints[1].y; y++){
            int top_segment_height = screenPoints[1].y - screenPoints[0].y + 1;

            float alpha = (float)(y - screenPoints[0].y) / triangle_height;
            assert(top_segment_height != 0);
            float beta  = (float)(y - screenPoints[0].y) / top_segment_height;

            Point A = screenPoints[0] + (screenPoints[2] - screenPoints[0]) * alpha;
            Point B = screenPoints[0] + (screenPoints[1] - screenPoints[0]) * beta;

            if(A.x > B.x) std::swap(A, B);           
            for(int horiz=A.x; horiz<=B.x; horiz++){
                if(horiz >= 0 && horiz < SCREEN_WIDTH-1){
                    float z = triangle.solve_plane_for_z(Point(horiz, y));
                    if(z < zbuffer[PIXEL_OFFSET(horiz, y)]){
                        zbuffer[PIXEL_OFFSET(horiz, y)] = z;
                        setPixel(pixels, horiz, y, color);
                    }
                }
            }
        }

        //draw bottom half
        for(int y = screenPoints[1].y; y <= screenPoints[2].y; y++){
            int bottom_segment_height = screenPoints[2].y - screenPoints[1].y + 1;

            float alpha = (float)(y - screenPoints[0].y) / triangle_height;
            assert(bottom_segment_height != 0);
            float beta  = (float)(y - screenPoints[1].y) / bottom_segment_height;

            Point A = screenPoints[0] + (screenPoints[2] - screenPoints[0]) * alpha;
            Point B = screenPoints[1] + (screenPoints[2] - screenPoints[1]) * beta;

            if(A.x > B.x) std::swap(A, B);           
            for(int horiz=A.x; horiz<=B.x; horiz++){
                if(horiz >= 0 && horiz < SCREEN_WIDTH-1){
                    float z = triangle.solve_plane_for_z(Point(horiz, y));
                    if(z < zbuffer[PIXEL_OFFSET(horiz, y)]){
                        zbuffer[PIXEL_OFFSET(horiz, y)] = z;
                        setPixel(pixels, horiz, y, color);
                    }
                }
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

