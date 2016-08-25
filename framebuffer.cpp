#include "framebuffer.hpp"
#include <algorithm>
#include <utility>
#include <vector>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VIEWPORT_WIDTH_3D 200
#define VIEWPORT_HEIGHT_3D 200
#define ZBUFFER_DEPTH 255

#define BETWEEN(token, A, B) (token >= A && token <= B)

#define M_PI 3.141592653589793238462643383279502884
#define DEG_TO_RAD(X) ((X * M_PI) / 180.0)

Framebuffer::Framebuffer(){
    pixels = (Pixel*)malloc(64000+1);
    if(pixels == NULL){
        printf("malloc() failed to allocate framebuffer\n");
        exit(255);
    }
    memset(pixels, 0, VGA_SIZE);

    zbuffer = (float*)malloc(sizeof(float)*64000+1);
    for(int i=0;i<64000+1;i++){
        zbuffer[i] = 200.0; //TODO: negative infinity
    }
}

void Framebuffer::reset_zbuffer(){
    for(int i=0;i<64000+1;i++){
        zbuffer[i] = 200.0; //TODO: negative infinity
    }
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
            if(BETWEEN(x, 0, SCREEN_HEIGHT) && BETWEEN(y, 0, SCREEN_WIDTH)){
                setPixel(y, x, color);
                verticesOnScanline[x].push_back(y);
            }
        }
        else
        {
            if(BETWEEN(x, 0, SCREEN_WIDTH) && BETWEEN(y, 0, SCREEN_HEIGHT)){
                setPixel(x, y, color);
                verticesOnScanline[y].push_back(x);
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

Point rotate(Point point, int rotation_degrees){
    double s = sin(rotation_degrees * (PI / 180.0));
    double c = cos(rotation_degrees * (PI / 180.0));

    double rotatedX = point.getX() * c - point.getY() * s;
    double rotatedY = point.getX() * s + point.getY() * c;

    return Point(rotatedX, rotatedY);
}

bool Framebuffer::draw_face(SceneObject obj, Vector3f eye, Vector3f cameraRotation, int face_number){
    Face face = obj.model.getFaces()[face_number];
    float zNear = 1.0;
    float zFar = 50.0;

    //Eye is the camera transform. the "camera" should point at Center.
    //so we need to rotate the scene so Center is in the center of the screen

    Vector3f worldCoords[3];
    worldCoords[0] = Vector3f(obj.model.getLocalVertices()[face.v1].x, obj.model.getLocalVertices()[face.v1].y, obj.model.getLocalVertices()[face.v1].z);
    worldCoords[1] = Vector3f(obj.model.getLocalVertices()[face.v2].x, obj.model.getLocalVertices()[face.v2].y, obj.model.getLocalVertices()[face.v2].z);
    worldCoords[2] = Vector3f(obj.model.getLocalVertices()[face.v3].x, obj.model.getLocalVertices()[face.v3].y, obj.model.getLocalVertices()[face.v3].z);

    Vector3f textureCoords[3];
    textureCoords[0] = Vector3f(obj.model.getTextureVertices()[face.v1].x, obj.model.getTextureVertices()[face.v1].y, obj.model.getTextureVertices()[face.v1].z);
    textureCoords[1] = Vector3f(obj.model.getTextureVertices()[face.v2].x, obj.model.getTextureVertices()[face.v2].y, obj.model.getTextureVertices()[face.v2].z);
    textureCoords[2] = Vector3f(obj.model.getTextureVertices()[face.v3].x, obj.model.getTextureVertices()[face.v3].y, obj.model.getTextureVertices()[face.v3].z);

    Vector3f transformedWorldCoords[3];
    Vector3f screenCoords[3];
    Vector3f clipCoords[3];

    //Scale -> Rotate -> Translate    
    for(int i=0;i<3;i++){
        transformedWorldCoords[i] = applyTransformations(worldCoords[i], 
                                                         eye,
                                                         cameraRotation,
                                                         obj.transformation.translation,
                                                         obj.transformation.rotation,
                                                         obj.transformation.scale);

        //Scale Z to (0,1)
        float scaledZ = (transformedWorldCoords[i].z - zNear) / (zFar - zNear);

        //are we drawing a triangle outside of our clipping planes?
        if(scaledZ >= 1.0 || scaledZ <= 0.0){
            return false;
        }

        float vFov = DEG_TO_RAD(64.0);

        //before we perspective divide, these are clipping coordinates
        clipCoords[i].x = screenCoords[i].x;
        clipCoords[i].y = screenCoords[i].y;
        clipCoords[i].z = screenCoords[i].z;

        //Perspective divide
        screenCoords[i].x = transformedWorldCoords[i].x / transformedWorldCoords[i].z;
        screenCoords[i].y = transformedWorldCoords[i].y / transformedWorldCoords[i].z;
        screenCoords[i].z = transformedWorldCoords[i].z;

        //Perform scaling
        float scaleFactor = 200.0f;
        screenCoords[i].x = screenCoords[i].x * scaleFactor + SCREEN_WIDTH/2;
        screenCoords[i].y = -screenCoords[i].y * scaleFactor + SCREEN_HEIGHT/2;
    }

    Vector3f faceNormal = calculateSurfaceNormal(transformedWorldCoords).normalize();
    Vector3f vectorCameraToTriangle = center - transformedWorldCoords[0];
    float triangleFacing = Vector3f::dot_product(vectorCameraToTriangle.normalize(), faceNormal);

    if(triangleFacing > 0.0f){
        Vector3f lightDirection = Vector3f(0,0,-1);
        float lightIntensity = faceNormal * lightDirection;
        int lightLevel = (lightIntensity * 128.0) / 16.0;
        lightLevel = std::min(lightLevel, 0x18);

        draw_projected_triangle(pixels, zbuffer, Triangle(screenCoords[0], screenCoords[1], screenCoords[2], textureCoords[0], textureCoords[1], textureCoords[2], clipCoords[0], clipCoords[1], clipCoords[2]).sortByY(), std::max(0x13, 0x10 + lightLevel), true);
        return true;
    } else {
        return false;
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

inline void Framebuffer::setPixel(int x, int y, int color){
    pixels[VGA_Y_OFFSETS[y] + x] = color;
}

inline void Framebuffer::setPixel(Point point, int color){
    //Set an individual pixel to a color.
    pixels[VGA_Y_OFFSETS[point.getY()] + point.getX()] = color; 
}

void Framebuffer::overlay(Framebuffer source, int size){
    //Copy one framebuffer on top of another.
    //Any pixels in the source of color 0x00 will be considered transparent.
    
    for(UInt32 i=0;i<size;i++){
        if(source.pixels[i] != 0x00){
            pixels[i] = source.pixels[i];
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

Vector3f applyTransformations(Vector3f v, Vector3f eye, Vector3f cameraRotation, Vector3f translation, Vector3f rotation, Vector3f scale){
    Vector3f result = Vector3f(v.x, v.y, v.z);

    //Scale
    result = Vector3f(result.x * scale.x,
                      result.y * scale.y,
                      result.z * scale.z);

    //Rotate - Intrinsic object rotation
    result = result.rotateAroundZAxis(rotation.z);
    result = result.rotateAroundXAxis(rotation.x);
    result = result.rotateAroundYAxis(rotation.y);

    //Translate
    //apply world translation
    result = Vector3f(result.x - eye.x,
                      result.y - eye.y,
                      result.z - eye.z);

    //then apply local translation
    result = Vector3f(result.x + translation.x,
                      result.y + translation.y,
                      result.z + translation.z);

    //then perform extrinsic camera rotation
    result = result.rotateAroundYAxis(cameraRotation.y);
    result = result.rotateAroundXAxis(cameraRotation.x);
    result = result.rotateAroundZAxis(cameraRotation.z);

    return result;
}

Vector3f calculateSurfaceNormal(Vector3f *triangle){
    Vector3f U = triangle[1] - triangle[0];
    Vector3f V = triangle[2] - triangle[0];

    return Vector3f::cross_product(U, V);
}
