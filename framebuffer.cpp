#include "framebuffer.hpp"
#include <algorithm>
#include <utility>
#include <vector>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define VIEWPORT_WIDTH_3D 200
#define VIEWPORT_HEIGHT_3D 200
#define ZBUFFER_DEPTH 255

Framebuffer::Framebuffer(){
    pixels = (Pixel*)malloc(64000+1);
    if(pixels == NULL){
        printf("malloc() failed to allocate framebuffer\n");
        exit(255);
    }
    memset(pixels, 0, VGA_SIZE);

    zbuffer = (float*)malloc(sizeof(float)*64000+1);
    for(int i=0;i<64000+1;i++){
        zbuffer[i] = -100.0; //TODO: negative infinity
    }
}

void Framebuffer::reset_zbuffer(){
    for(int i=0;i<64000+1;i++){
        zbuffer[i] = -100.0; //TODO: negative infinity
    }
}

Pixel *Framebuffer::getPixels(){
    return pixels;
}

void swap(int &a, int &b){
    int c = a;
    a = b;
    b = c;
}

std::vector<int> verticesOnScanline[200];

void Framebuffer::draw_line(Point start, Point end, int color){
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
            setPixel(y, x, color);
            verticesOnScanline[x].push_back(y);
        }
        else
        {
            setPixel(x, y, color);
            verticesOnScanline[y].push_back(x);
        }
 
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }

    }
}

Point rotate(Point point, int rotation_degrees){
    double s = sin(rotation_degrees * (PI / 180.0));
    double c = cos(rotation_degrees * (PI / 180.0));

    double rotatedX = point.getX() * c - point.getY() * s;
    double rotatedY = point.getX() * s + point.getY() * c;

    return Point(rotatedX, rotatedY);
}

Vector3f calculateSurfaceNormal(Vector3f *triangle){
    Vector3f U = triangle[2] - triangle[0];
    Vector3f V = triangle[1] - triangle[0];

    return Vector3f::cross_product(U, V);
}

void Framebuffer::draw_face(WavefrontObject model, Matrix Viewport, Matrix Projection, Matrix ModelView, int face_number){

    //camera distance from origin in units
    float c = 5.0;
    
    Vector3f light_direction(0.0, 0.0, -1.0);
    Face face = model.getFaces()[face_number];

    Vector3f worldCoords[3];
    worldCoords[0] = Vector3f(model.getLocalVertices()[face.v1].x, model.getLocalVertices()[face.v1].y, model.getLocalVertices()[face.v1].z);
    worldCoords[1] = Vector3f(model.getLocalVertices()[face.v2].x, model.getLocalVertices()[face.v2].y, model.getLocalVertices()[face.v2].z);
    worldCoords[2] = Vector3f(model.getLocalVertices()[face.v3].x, model.getLocalVertices()[face.v3].y, model.getLocalVertices()[face.v3].z);

    Matrix transformation = Viewport * Projection * ModelView;

    Vector3f screenCoords[3];
    //Apply view transformations
    //Viewport * Projection * ModelView * coordinates
    for(int i=0;i<3;i++){
        Matrix newCoords = transformation * Matrix::position_vector(worldCoords[i]);
        //Get screen coordinates from the perspective transformation
        screenCoords[i] = Vector3f(newCoords[0][0] / newCoords[3][0], newCoords[1][0] / newCoords[3][0], newCoords[2][0] / newCoords[3][0]);
}

    //Current issue is that we need to calculate the new normal
    //based on the eye location
    //http://www.songho.ca/opengl/gl_normaltransform.html
    Vector3f normal = calculateSurfaceNormal(worldCoords);
    Vector3f normalizedNormal = normal.normalize();

    Matrix newNormal = transformation.transpose().inverse() * Matrix::direction_vector(normal);
    Vector3f transposedNormal = Vector3f(newNormal[0][0], newNormal[1][0], newNormal[2][0]);
    
    float intensity = transposedNormal * light_direction;
    float lightLevel = (intensity * 127) / 16;
    lightLevel = lightLevel > 0x0F ? 0x0F : lightLevel;

    if(lightLevel > 0) {
        draw_filled_triangle(Triangle(screenCoords[0], screenCoords[1], screenCoords[2]),
                                worldCoords,
                                Point(0,0),
                                0x10 + lightLevel);
        //transformation.print();
        //printf("old normal: %f %f %f\n", normal.x, normal.y, normal.z);
        //newNormal.print();
    } else {
        //printf("Face %d is too dark\n", face_number);
    }
}

void Framebuffer::draw_triangle(Triangle triangle, Point origin, int color){
    for(int i=0;i<3;i++){
        Vector3f new_start = vector_offset(triangle.getPoint(i), origin);
        Vector3f new_end = vector_offset(triangle.getPoint((i+1) % 3), origin);
        draw_line((Point2D)new_start, (Point2D)new_end, color);
    }
}

Vector3f barycentric_point(Point A, Point B, Point C, Point P){    
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

void Framebuffer::draw_filled_triangle(Triangle triangle, Vector3f worldCoordinates[], Point origin, int color){
    for(int i=0;i<200;i++){
        verticesOnScanline[i].clear();
    }

    Point screenPoints[3];
    for(int i=0;i<3;i++){
        screenPoints[i] = Point((int)triangle.getPoints()[i].x + .5, (int)triangle.getPoints()[i].y + .5);
    }

    //printf("draw_filled_triangle: triangle is (%d,%d),(%d,%d),(%d,%d)\n", screenPoints[0].x, screenPoints[0].y, screenPoints[1].x, screenPoints[1].y, screenPoints[2].x, screenPoints[2].y);

    int boundingBoxX[2] = {321, -1};
    int boundingBoxY[2] = {201, -1};

    //find the bounding box of the triangle
    for(int i=0; i<3; i++){
        if(screenPoints[i].x < boundingBoxX[0]) boundingBoxX[0] = screenPoints[i].x;
        if(screenPoints[i].x > boundingBoxX[1]) boundingBoxX[1] = screenPoints[i].x;

        if(screenPoints[i].y < boundingBoxY[0]) boundingBoxY[0] = screenPoints[i].y;
        if(screenPoints[i].y > boundingBoxY[1]) boundingBoxY[1] = screenPoints[i].y;
    }

    if(boundingBoxX[0] >= 0 && boundingBoxX[1] < 320 && boundingBoxY[0] >= 0 && boundingBoxY[1] < 200){
        for(int scanline=boundingBoxY[0]; scanline<=boundingBoxY[1]; scanline++){
            //printf("Scanline %d\n", scanline);
            for(int x=std::max(0, boundingBoxX[0]);x<=std::min(320, boundingBoxX[1]);x++){               
                Vector3f barycentric = barycentric_point(screenPoints[0], screenPoints[1], screenPoints[2], Point(x, scanline));
                if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;
                
                float z = 0;
                z += (worldCoordinates[0].z * barycentric.x);
                z += (worldCoordinates[1].z * barycentric.y);
                z += (worldCoordinates[2].z * barycentric.z);
                if(z > zbuffer[x + (scanline * VGA_WIDTH)]) {
                    zbuffer[x + (scanline * VGA_WIDTH)] = z;
                    setPixel(x, scanline, color);
                }
            }
        }
        for(int i=0;i<3;i++){
            Point start = triangle.getPoint(i);
            Point end = triangle.getPoint((i+1) % 3);
            draw_line(start, end, COLOR_GREEN);
        }
    }
}

void Framebuffer::draw_rectangle(Point origin, int width, int height, int thickness, int color){
    //Draw a rectangle at point Origin of Height*Width area Thickness pixels thick.
    for(int cur_thickness=0;cur_thickness<thickness;cur_thickness++){
        Point topLeft       = Point(origin.getX() + cur_thickness, origin.getY() + cur_thickness);
        Point topRight      = Point(origin.getX() + width - cur_thickness, origin.getY() + cur_thickness);
        Point bottomLeft    = Point(origin.getX() + cur_thickness, origin.getY() + height - cur_thickness);
        Point bottomRight   = Point(origin.getX() + width - cur_thickness, origin.getY() + height - cur_thickness);
        
        draw_line(topLeft, topRight, color);
        draw_line(topLeft, bottomLeft, color);
        draw_line(bottomLeft, bottomRight, color);
        draw_line(topRight, bottomRight, color);   
    }
}

void Framebuffer::draw_rectangle_filled(Point origin, int width, int height, int color){ 
    for(int i=0;i<height;i++){
        draw_line(Point(origin.getX(), origin.getY() + i), Point(origin.getX() + width, origin.getY() + i), color);
    }
}

void Framebuffer::setPixel(int x, int y, int color){
    unsigned int coord = VGA_Y_OFFSETS[y] + x;
    pixels[coord] = color;
}


void Framebuffer::setPixel(Point point, int color){
    //Set an individual pixel to a color.
    pixels[VGA_Y_OFFSETS[point.getY()] + point.getX()] = color; 
}

void Framebuffer::overlay(Framebuffer source, int size){
    //Copy one framebuffer on top of another.
    //Any pixels in the source of color 0x00 will be considered transparent.
    
    for(UInt32 i=0;i<size;i++){
        if(source.getPixels()[i] != 0x00){
            pixels[i] = source.getPixels()[i];
        }
    }
}

void Framebuffer::clear_area(Point start, Size2D size){
    int i=0;

    uint16_t startx = start.getX();
    uint16_t starty = start.getY();

    uint16_t sizex = size.getX();
    uint16_t sizey = size.getY();

    uint16_t endx = startx + sizex;
    uint16_t endy = starty + sizey;
    
    for(int y = starty; y < endy; y++){
        for(int x = startx; x < endx; x++){
            pixels[VGA_Y_OFFSETS[y] + x] = 0x00;
            i++;
        }
    }
}

void Framebuffer::draw_area(const Pixel *source, Point start, Size2D size){
    int i=0;

    uint16_t startx = start.getX();
    uint16_t starty = start.getY();

    uint16_t sizex = size.getX();
    uint16_t sizey = size.getY();

    uint16_t endx = startx + sizex;
    uint16_t endy = starty + sizey;
    
    for(int y = starty; y < endy; y++){
        for(int x = startx; x < endx; x++){
            if(source[i] != COLOR_TRANSPARENT){
                pixels[VGA_Y_OFFSETS[y] + x] = source[i];
            }
            i++;
        }
    }
}

double getSlope(Point start, Point end){
    //m = (startY - endY) / (startX - endX)
    return (double)(start.getY() - end.getY()) / (start.getX() - end.getX());
}

double getYIntercept(Point start, double slope){
    //Where will a line of slope M that passes through point (X,Y) pass through Y=0?
    return start.getY() - (slope * start.getX());
}

Point rotate_degrees(Point point, int rotation_degrees){
    printf("\n%f, %f", point.getX(), point.getY());
    double s = sin(rotation_degrees * (PI / 180.0));
    double c = cos(rotation_degrees * (PI / 180.0));
    
    Point rotated = Point((point.getX() * c) + (point.getY() * s), (-point.getX() * s) + (point.getY() * c));
    printf("\n%f, %f -> %f, %f\n", point.getX(), point.getY(), rotated.getX(), rotated.getY());

    return rotated;
}

/* Font routines */
void Framebuffer::putGlyph(Pixel *tile, int sizeX, int sizeY, int destX, int destY, int vga_color){ //8x8 font tile   
    long bytePos = (destY * 320) + destX;

    for(int y=0; y<sizeY; y++) {
        //for each row of the tile
        if(tile[y] & 0x80) {
            pixels[bytePos] = vga_color;
        } else {
            pixels[bytePos] = 0x00;
        }

        if(tile[y] & 0x40) {
            pixels[bytePos+1] = vga_color;
        } else {
            pixels[bytePos+1] = 0x00;
        }

        if(tile[y] & 0x20) {
            pixels[bytePos+2] = vga_color;
        } else {
            pixels[bytePos+2] = 0x00;
        }
        
        if(tile[y] & 0x10) {
            pixels[bytePos+3] = vga_color;
        } else {
            pixels[bytePos+3] = 0x00;
        }
        
        if(tile[y] & 0x08) {
            pixels[bytePos+4] = vga_color;
        } else {
            pixels[bytePos+4] = 0x00;
        }
        
        if(tile[y] & 0x04) {
            pixels[bytePos+5] = vga_color;
        } else {
            pixels[bytePos+5] = 0x00;
        }
        
        if(tile[y] & 0x02) {
            pixels[bytePos+6] = vga_color;
        } else {
            pixels[bytePos+6] = 0x00;
        }
        
        if(tile[y] & 0x01) {
            pixels[bytePos+7] = vga_color;
        } else {
            pixels[bytePos+7] = 0x00;
        }
        
        bytePos += 320;
    }
    
};

void Framebuffer::putString(const char *str, int len, Point destination, int vga_color, Font font){
    int sizeX = font.getSizeX();
    int sizeY = font.getSizeY();

    for(int i=0;i<len;i++){
        putGlyph(font.getGlyph(str[i]), sizeX, sizeY, destination.getX()+(sizeX * i), destination.getY(), vga_color);
    }
}

void Framebuffer::putString(std::string str, Point destination, int color, Font font){
    putString(str.c_str(), str.length(), destination, color, font);
}
