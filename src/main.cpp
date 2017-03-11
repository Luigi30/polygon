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
    PTR_SCENEOBJECT("player")->transformation.translation = _eye;
    PTR_SCENEOBJECT("player")->transformation.rotation = _cameraRotation;

}

int main(){
    load_lookup_tables();   //needed for any draw routine
    Install_KBHook();       //needed for input processing

    //Debug_Init();
    //init_timer(11930); //100 ticks per second
    serialPort = Serial(0x3F8, SER_9600_BAUD);
    printf("main(): serialPort setup\n");
    serialPort.sendString("\n\n\r\n*** CONNECTED ***\r\n");
    serialPort.sendString("main(): Serial connected\r\n");
    
    VGA_PTR = (char*)VGA_LINEAR_ADDRESS;
    _setvideomode(_MRES256COLOR); //Change to mode 13h
    //MainMenu();
    BeginSimulation();

    //exit cleanup routines
    Restore_KBHook();
    _setvideomode(_DEFAULTMODE);
    printf("exiting!\n");
    return 0;
}

void MainMenu(){
    Mouse::cursorEnable();

    controlsState.escapePressed = false;

    g_screen.layer_background.draw_rectangle_filled(Point(80, 50), Size2D(160, 100), COLOR_LTGRAY);
    g_screen.layer_background.shade_rectangle(Point(80, 50), Size2D(160, 100), COLOR_WHITE, COLOR_DKGRAY);
    g_screen.layer_text.putString("Main Menu", strlen("Main Menu"), Point(85, 55), COLOR_GRAYS, FONT_6x8);

    g_screen.widgetsList.push_back(new W_Button("btn_Launch", Point(85, 70), BUTTON_SHAPE_RECT, Size2D(30, 50), "", true));

    while(controlsState.escapePressed == false){
        wait_for_vsync();
        controlsState.DecayAxes();
        
        //Process inputs
        read_keyboard();

        Mouse::cursorDisable();
        g_screen.redraw();
        Mouse::cursorEnable();
    }

    //BeginSimulation();
}

void BeginSimulation(){
    _setvideomode(_DEFAULTMODE);

    std::printf("Polysim Engine\n");
    std::printf("Now loading...\n");
    std::printf("Connect 9600,8,N,1 terminal to COM1\n");

    printf("Loading models\n");
    WavefrontObject cube = WavefrontObject("cube.3d");
    WavefrontObject ship = WavefrontObject("sqrship.3d");

    /*
    g_SceneManager.addSceneObject("ship", ship, Vector3f(0,0,0), Vector3f(0,0,0), Vector3f(1,1,1));
    PTR_SCENEOBJECT("ship")->transformation.rotation = Vector3f(0,0,0);
    PTR_SCENEOBJECT("ship")->movement.desired_rotation = Vector3f(0,0,0);
    */

    ptr_DumbShip dumbShip(new DumbShip);
    dumbShip->name = "ship";
    dumbShip->model = ship;
    dumbShip->transformation.translation = Vector3f(0,0,0);
    dumbShip->transformation.rotation = Vector3f(0,0,0);
    dumbShip->transformation.scale = Vector3f(1,1,1);  
    dumbShip->can_think = true;  
    g_SceneManager.addSceneObject(dumbShip);

    g_SceneManager.addSceneObject("player", cube, Vector3f(0,0,-5), Vector3f(0,0,0), Vector3f(.5,.5,.5));
    PTR_SCENEOBJECT("player")->transformation.rotation = Vector3f(0,0,0);
    PTR_SCENEOBJECT("player")->movement.desired_rotation = Vector3f(0,0,0);

    _setvideomode(_MRES256COLOR); //Change to mode 13h

    g_screen.draw_polygon_debug_data();
    g_screen.draw_object_debug_data(*PTR_SCENEOBJECT("ship"));
    g_screen.redraw();

    bool stop = false;
    controlsState.escapePressed = false;

    serialPort.sendString("main(): Beginning simulation\r\n");

    //Scene loop
    while(!stop){
        stop = DoSceneLoop();
    }

}

bool DoSceneLoop(){
    //we need a player object
    assert(PTR_SCENEOBJECT("player") != NULL);

    //Limit to 35Hz refresh
    wait_for_vsync();
    
    //clamp rotation
    PTR_SCENEOBJECT("ship")->transformation.rotation.x = std::fmod(PTR_SCENEOBJECT("ship")->transformation.rotation.x, 360.0f);
    PTR_SCENEOBJECT("ship")->transformation.rotation.y = std::fmod(PTR_SCENEOBJECT("ship")->transformation.rotation.y, 360.0f);

    //SceneObjects now think.
    g_SceneManager.objectsThink();

    //update object location and orientation
    g_SceneManager.applyObjectVelocities();
    g_SceneManager.applyObjectRotations();

    //draw some debug data
    g_screen.draw_polygon_debug_data();
    g_screen.draw_object_debug_data(*PTR_SCENEOBJECT("ship"));
    g_screen.redraw();

    //Process inputs, update controlsState
    read_keyboard();

    //Update player's forward speed
    PTR_SCENEOBJECT("player")->movement.forward_speed = controlsState.forward_throttle * PTR_SCENEOBJECT("player")->movement.maximum_throttle_speed;
    //Apply this speed to the direction vector
    Vector3f direction = controlsState.direction + Vector3f(0, 0, PTR_SCENEOBJECT("player")->movement.forward_speed);
    
    //Rotate direction to align with the camera
    cameraRotation.z = controlsState.rotation.z;
    cameraRotation.x = controlsState.rotation.x;
    cameraRotation.y = controlsState.rotation.y;

    //Update global transformation matrix with the player's location and rotation
    eye = PTR_SCENEOBJECT("player")->transformation.translation;     
    direction = direction.rotateAroundZAxis(-cameraRotation.z);
    direction = direction.rotateAroundXAxis(-cameraRotation.x);
    direction = direction.rotateAroundYAxis(-cameraRotation.y);
    eye = eye + direction;
    
    updatePlayerPosition(eye, cameraRotation);

    return controlsState.escapePressed;
}