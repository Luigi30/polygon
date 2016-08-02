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
/*
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
*/

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

    //Trigonometry tables
    for(int i=0;i<360;i++){
        SIN_TABLE[i] = std::sin(i * (M_PI/180.0f));
        COS_TABLE[i] = std::cos(i * (M_PI/180.0f));
    }
}

int main(){
    //init_timer(11930); //100 ticks per second
    
    VGA_PTR = (char*)VGA_LINEAR_ADDRESS;
    debugOutput("OK\r\n");
    
    std::printf("Polygon\n");
    load_lookup_tables();

    printf("Loading models\n");

    WavefrontObject cube = WavefrontObject("cube.3d");

    g_screen.addSceneObject("head", cube, Vector3f(0,0,0), Vector3f(0,0,0), Vector3f(1,1,1));
    g_screen.getSceneObjectPtr("head")->transformation.rotation = Vector3f(0,0,0);
    g_screen.getSceneObjectPtr("head")->movement.desired_rotation = Vector3f(0,0,0);

    g_screen.addSceneObject("player", cube, Vector3f(0,0,0), Vector3f(0,0,0), Vector3f(.5,.5,.5));
    g_screen.getSceneObjectPtr("player")->transformation.rotation = Vector3f(0,0,0);
    g_screen.getSceneObjectPtr("player")->movement.desired_rotation = Vector3f(0,0,0);

    _setvideomode(_MRES256COLOR); //Change to mode 13h

    g_screen.draw_polygon_debug_data();
    g_screen.draw_object_debug_data(*g_screen.getSceneObjectPtr("head"));
    g_screen.redraw();

    bool abort = false;
    bool goForward = false;
    int mode7_angle = 0;

    //Scene loop
    while(!abort){

        //we need a player object
        assert(g_screen.getSceneObjectPtr("player") != NULL);

        //Limit to 35Hz refresh
        wait_for_vsync();
        //wait_for_vsync();
        
        //clamp rotation
        g_screen.getSceneObjectPtr("head")->transformation.rotation.x = std::fmod(g_screen.getSceneObjectPtr("head")->transformation.rotation.x, 360.0f);
        g_screen.getSceneObjectPtr("head")->transformation.rotation.y = std::fmod(g_screen.getSceneObjectPtr("head")->transformation.rotation.y, 360.0f);

        //update object location and orientation
        g_screen.applyObjectVelocities();
        g_screen.applyObjectRotations();

        //draw some debug data
        g_screen.draw_polygon_debug_data();
        g_screen.draw_object_debug_data(*g_screen.getSceneObjectPtr("head"));
        g_screen.mode7_background(mode7_angle, 1.0f);
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
                cameraRotation.x = std::fmod(cameraRotation.x - ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'v'){
                cameraRotation.x = std::fmod(cameraRotation.x + ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'u'){
                cameraRotation.z = std::fmod(cameraRotation.z - ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'i'){
                cameraRotation.z = std::fmod(cameraRotation.z + ROTATION_DEGREES_PER_STEP, 360.0f);
            }
            else if(key == 'f'){
                g_screen.getSceneObjectPtr("head")->transformation.rotation.y += 5.0;
            }
            else if (key == 'g'){
                goForward = ~goForward;
            }
            else if (key == 'h'){
                g_screen.getSceneObjectPtr("head")->movement.desired_rotation = Vector3f(15, 30, 0);
            }
            else if (key == 'j'){
                g_screen.getSceneObjectPtr("head")->movement.desired_rotation = Vector3f(10, -25, 0);
            }
            else if (key == 'k'){
                g_screen.getSceneObjectPtr("head")->movement.forward_speed = 0.05f;
            }
        }

        if(goForward) {
            /*
            SceneObject *obj = g_screen.getSceneObjectPtr("head");
            Vector3f delta = obj->forward_vector() * 0.1f;
            obj->transformation.translation = obj->transformation.translation + delta;
            */
            mode7_angle++;
        }
        
        //eye = g_screen.getSceneObjectPtr("player")->transformation.translation;       
        direction = direction.rotateAroundZAxis(-cameraRotation.z);
        direction = direction.rotateAroundXAxis(-cameraRotation.x);
        direction = direction.rotateAroundYAxis(-cameraRotation.y);
        eye = eye + direction;
        
        g_screen.getSceneObjectPtr("player")->transformation.translation = eye;
        g_screen.getSceneObjectPtr("player")->transformation.rotation = cameraRotation;
    }

    _setvideomode(_DEFAULTMODE);

    printf("exiting!\n");
    return 0;

}