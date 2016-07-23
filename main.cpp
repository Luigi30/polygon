#include "main.hpp"
#include <cmath>
#include <time.h>

#define M_PI 3.141592653589793238462643383279502884
#define DEG_TO_RAD(X) ((X * M_PI) / 180.0)

#define ROTATION_DEGREES_PER_STEP 5

std::vector<Shape> shapesList;

void wait_for_vsync(){
    //Wait for the end of a retrace
    do {} while(inp(0x3DA) & 0x08);

    //Wait for the new retrace
    do {} while (!(inp(0x3DA) & 0x08));
}

void abort(char *msg){
    printf("\r\nCritical error: %s\r\n", msg);
    exit(255);
}

void init_timer(int rate){
    //The timer runs at 1193180Hz
    //Timer frequency = 1193180/rate
    
    //Initialize our timer
    myTimerTicks = 0;
    
    long timerReload = rate;
    biosTimerHandler = _dos_getvect(0x08); //DOS timer handler

    _disable();

    //_dos_setvect(0x08, timerHandler);
    //dpmi_set_pm_handler(0x08, timerHandler);
    
    outp(0x43, 0x34);
    outp(0x40, timerReload & 0xFF);
    outp(0x40, timerReload >> 8);
    _enable();
}

void load_shapes(){
    //Read SHAPES.DAT
    FILE *hShapesFile;
    hShapesFile = fopen("SHAPES.DAT", "r");

    char rawShapes[64][4];

    int numShapes = 0;

    if(hShapesFile != NULL){
        while(fgets(rawShapes[numShapes], 255, hShapesFile) != NULL){
            numShapes++;
        }
    } else {
        abort("SHAPES.DAT not found.");
    }

    for(int i=0;i<numShapes;i++){        
        shapesList.push_back(Shape("NEWSHAPE", rawShapes[i])); //TODO: shape names
    }
}

void load_lookup_tables(){
    //VGA_Y_OFFSETS - offset of the start of a new row of pixels
    for(int i=0;i<VGA_HEIGHT;i++){
        VGA_Y_OFFSETS[i] = i * VGA_WIDTH;
    }
}

Point transform_2d(Matrix transformation, Point p){
    //We need a 3x3 matrix to transform a point
    assert(transformation.numRows() == 3 && transformation.numColumns() == 3);
    Matrix result = transformation * Matrix::vector(p.x, p.y, 1); //Vector3f(p.x, p.y, 1);
    printf("(%d,%d) -> (%f,%f,%f)\n", p.x, p.y, result.X(), result.Y(), result.Z());
    return Point(result.X()/result.Z(), result.Y()/result.Z());
}

int main(){
    init_timer(11930); //100 ticks per second
    
    VGA_PTR = (char*)VGA_LINEAR_ADDRESS;
    debugOutput("OK\r\n");
    
    std::printf("Polygon\n");
    load_lookup_tables();

    printf("Loading model\n");

    WavefrontObject obj;
    obj.load_file("cube.3d");
    obj.translation = Vector3f(0,0,0);
    obj.rotation = Vector3f(0,0,0);
    obj.scale = Vector3f(1,1,1);
    g_screen.addSceneObject("cube1", obj);

    WavefrontObject ship;
    obj.load_file("sqrship.3d");
    obj.translation = Vector3f(0,0,5);
    obj.scale = Vector3f(1,1,1);
    obj.velocity = Vector3f(0,0,0.02);
    g_screen.addSceneObject("ship", obj);
    
    printf("OK\n");

    printf("Found %d vertices\n", obj.getVertexCount());
    printf("Found %d faces\n", obj.getFaceCount());

    _setvideomode(_MRES256COLOR); //Change to mode 13h

    //recalculate_transformation();
    g_screen.draw_polygon_debug_data();
    //g_screen.draw_view_data(View);
    //g_screen.draw_polygon_object(obj);
    g_screen.draw_object_debug_data(g_screen.getSceneObjectPtr("cube1")->model);
    g_screen.redraw();

    int rotationX = 0;
    int rotationY = 0;
    int rotationZ = 0;

    bool abort = false;
    bool goForward = false;

    //Game loop
    while(!abort){

        //Limit to 35Hz refresh
        wait_for_vsync();
        wait_for_vsync();
        
        //clamp rotation
        g_screen.getSceneObjectPtr("cube1")->model.rotation.x = std::fmod(g_screen.getSceneObjectPtr("cube1")->model.rotation.x, 360.0f);
        g_screen.getSceneObjectPtr("cube1")->model.rotation.y = std::fmod(g_screen.getSceneObjectPtr("cube1")->model.rotation.y, 360.0f);

        //apply velocity to objects
        g_screen.applyObjectVelocities();

        g_screen.draw_polygon_debug_data();
        g_screen.draw_object_debug_data(g_screen.getSceneObjectPtr("cube1")->model);
        g_screen.redraw();

        Vector3f direction = Vector3f(0,0,0);

        if(kbhit()){
            char key = getch();

            if(key == 0x1B){ //ESC
                abort = true;
            }
            else if(key == 'a'){
                direction = Vector3f(-1,0,0);
            }
            else if(key == 'd'){
                direction = Vector3f(1,0,0);
            }
            else if(key == 'w'){
                direction = Vector3f(0,0,1);
            }
            else if(key == 's'){
                direction = Vector3f(0,0,-1);
            }
            else if(key == 'e'){
                cameraRotation.y = std::fmod(cameraRotation.y - ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'q'){
                cameraRotation.y = std::fmod(cameraRotation.y + ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'r'){
                cameraRotation.x += std::fmod((float)std::cos(DEG_TO_RAD(cameraRotation.y)), 360.0f) * ROTATION_DEGREES_PER_STEP;
            }
            else if(key == 'v'){
                cameraRotation.x -= std::fmod((float)std::cos(DEG_TO_RAD(cameraRotation.y)), 360.0f) * ROTATION_DEGREES_PER_STEP;
            }
            else if (key == 'g'){
                goForward = ~goForward;
            }
            else if (key == 'h'){
                g_screen.getSceneObjectPtr("cube1")->model.rotation.y = 5.0f + std::fmod(g_screen.getSceneObjectPtr("cube1")->model.rotation.y, 360.0f);
            }
        }

        if(goForward) {
            direction = direction + Vector3f(0,0,0.05);
        }

        direction = direction.rotateAroundXAxis(-cameraRotation.x);
        direction = direction.rotateAroundYAxis(-cameraRotation.y);
        direction = direction.rotateAroundZAxis(-cameraRotation.z);
        eye = eye + direction;
    }

    _setvideomode(_DEFAULTMODE);

    printf("exiting!\n");
    return 0;

}