#include "main.hpp"

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

void updatePlayerPosition(Vector3f _eye, Vector3f _cameraRotation){
    //Player's position and rotation = camera, effectively
    g_screen.getSceneObjectPtr("player")->transformation.translation = _eye;
    g_screen.getSceneObjectPtr("player")->transformation.rotation = _cameraRotation;

}

int main(){
    //Debug_Init();
    //init_timer(11930); //100 ticks per second
    serialPort = Serial(0x3F8, SER_9600_BAUD);
    printf("main(): serialPort setup.");
    serialPort.sendString("\n\n\r\n*** CONNECTED ***\r\n");
    serialPort.sendString("main(): Serial connected\r\n");
    
    VGA_PTR = (char*)VGA_LINEAR_ADDRESS;
    
    std::printf("Polygon\n");
    load_lookup_tables();

    printf("Loading models\n");
    serialPort.sendString("main(): Loading models\r\n");

    WavefrontObject cube = WavefrontObject("cube.3d");
    //WavefrontObject cube = WavefrontObject("triangle.3d");

    g_screen.addSceneObject("head", cube, Vector3f(0,0,0), Vector3f(0,0,0), Vector3f(1,1,1));
    g_screen.getSceneObjectPtr("head")->transformation.rotation = Vector3f(0,0,0);
    g_screen.getSceneObjectPtr("head")->movement.desired_rotation = Vector3f(0,0,0);

    g_screen.addSceneObject("player", cube, Vector3f(0,0,-5), Vector3f(0,0,0), Vector3f(.5,.5,.5));
    g_screen.getSceneObjectPtr("player")->transformation.rotation = Vector3f(0,0,0);
    g_screen.getSceneObjectPtr("player")->movement.desired_rotation = Vector3f(0,0,0);

    _setvideomode(_MRES256COLOR); //Change to mode 13h

    g_screen.draw_polygon_debug_data();
    g_screen.draw_object_debug_data(*g_screen.getSceneObjectPtr("head"));
    g_screen.redraw();

    bool stop = false;
    controlsState.escapePressed = false;

    serialPort.sendString("main(): Beginning simulation\r\n");

    //Scene loop
    while(!stop){
        stop = DoSceneLoop();
    }

    _setvideomode(_DEFAULTMODE);

    printf("exiting!\n");
    return 0;
}

bool DoSceneLoop(){
    //we need a player object
    assert(g_screen.getSceneObjectPtr("player") != NULL);

    //Limit to 35Hz refresh
    wait_for_vsync();
    
    //clamp rotation
    g_screen.getSceneObjectPtr("head")->transformation.rotation.x = std::fmod(g_screen.getSceneObjectPtr("head")->transformation.rotation.x, 360.0f);
    g_screen.getSceneObjectPtr("head")->transformation.rotation.y = std::fmod(g_screen.getSceneObjectPtr("head")->transformation.rotation.y, 360.0f);

    //update object location and orientation
    g_screen.applyObjectVelocities();
    g_screen.applyObjectRotations();

    //draw some debug data
    g_screen.draw_polygon_debug_data();
    g_screen.draw_object_debug_data(*g_screen.getSceneObjectPtr("head"));
    //g_screen.mode7_background(mode7_angle, 1.0f);
    g_screen.redraw();

    //Process inputs
    read_keyboard();

    //Update player's forward speed
    GAMEOBJECT("player")->movement.forward_speed = controlsState.forward_throttle * GAMEOBJECT("player")->movement.maximum_throttle_speed;
    //Apply this speed to the direction vector
    Vector3f direction = controlsState.direction + Vector3f(0, 0, GAMEOBJECT("player")->movement.forward_speed);
    
    //Rotate direction to align with the camera
    cameraRotation.z = controlsState.rotation.z;
    cameraRotation.x = controlsState.rotation.x;
    cameraRotation.y = controlsState.rotation.y;

    //Update transformation matrix with the player's location and rotation
    eye = GAMEOBJECT("player")->transformation.translation;       
    direction = direction.rotateAroundZAxis(-cameraRotation.z);
    direction = direction.rotateAroundXAxis(-cameraRotation.x);
    direction = direction.rotateAroundYAxis(-cameraRotation.y);
    eye = eye + direction;
    
    updatePlayerPosition(eye, cameraRotation);

    return controlsState.escapePressed;
}