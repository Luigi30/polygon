#include "main.hpp"
#include <cmath>

#define M_PI 3.141592653589793238462643383279502884
#define DEG_TO_RAD(X) ((X * M_PI) / 180.0)

std::vector<Shape> shapesList;

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
}

Point transform_2d(Matrix transformation, Point p){
    //We need a 3x3 matrix to transform a point
    assert(transformation.numRows() == 3 && transformation.numColumns() == 3);
    Matrix result = transformation * Matrix::vector(p.x, p.y, 1); //Vector3f(p.x, p.y, 1);
    printf("(%d,%d) -> (%f,%f,%f)\n", p.x, p.y, result.X(), result.Y(), result.Z());
    return Point(result.X()/result.Z(), result.Y()/result.Z());
}

int main(){
    VGA_PTR = (char*)VGA_LINEAR_ADDRESS;
    debugOutput("OK\r\n");
    
    std::printf("Polygon\n");
    load_lookup_tables();

    printf("Loading model\n");

    Projection = Matrix::identity(4);
    //Projection = perspective_projection(-1, 1, 0, 320, 1, 11);

    WavefrontObject obj;
    obj.load_file("cube.3d");
    obj.translation = Vector3f(-5,0,0);
    obj.scale = Vector3f(1,2.5,2.5);
    g_screen.addSceneObject("cube1", obj);

    WavefrontObject obj2;
    obj2.load_file("cube.3d");
    obj2.translation = Vector3f(5,0,0);
    obj2.scale = Vector3f(1,2.5,2.5);
    g_screen.addSceneObject("cube2", obj2);
    
    printf("OK\n");

    printf("Found %d vertices\n", obj.getVertexCount());
    printf("Found %d faces\n", obj.getFaceCount());

    //Transformation matrix
    //http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
    //Matrix transformation = Matrix::transformation(0,0,0);   
    //Matrix rotation = Matrix::identity(4);
    //Matrix scale = Matrix::scale(1, 1, 1);

    _setvideomode(_MRES256COLOR); //Change to mode 13h

    recalculate_transformation();
    g_screen.draw_polygon_debug_data();
    //g_screen.draw_view_data(View);
    //g_screen.draw_polygon_object(obj);
    g_screen.draw_object_debug_data(g_screen.getSceneObjectPtr("cube1")->model);
    g_screen.redraw();

    int rotationX = 0;
    int rotationY = 0;
    int rotationZ = 0;

    bool abort = false;

    while(!abort){
        delay(50);

        //g_screen.getSceneObjectPtr("cube1")->model.rotation = g_screen.getSceneObjectPtr("cube1")->model.rotation + Vector3f(0,5,0);
        //g_screen.getSceneObjectPtr("cube1")->model.rotation = g_screen.getSceneObjectPtr("cube1")->model.rotation + Vector3f(4,0,0);

        //clamp rotation
        g_screen.getSceneObjectPtr("cube1")->model.rotation.x = std::fmod(g_screen.getSceneObjectPtr("cube1")->model.rotation.x, 360.0f);
        g_screen.getSceneObjectPtr("cube1")->model.rotation.y = std::fmod(g_screen.getSceneObjectPtr("cube1")->model.rotation.y, 360.0f);

        g_screen.draw_polygon_debug_data();
        g_screen.draw_object_debug_data(g_screen.getSceneObjectPtr("cube1")->model);
        g_screen.redraw();

        if(kbhit()){
            char key = getch();

            Vector3f direction = Vector3f(0,0,0);

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
                cameraRotation.y = std::fmod(cameraRotation.y - 5, 360.0f);
            }
            else if(key == 'q'){
                cameraRotation.y = std::fmod(cameraRotation.y + 5, 360.0f);
            }
            else if(key == 'r'){
                cameraRotation.x = std::fmod(cameraRotation.x - 5, 360.0f);
            }
            else if(key == 'v'){
                cameraRotation.x = std::fmod(cameraRotation.x + 5, 360.0f);
            }

            direction = rotateAroundXAxis(direction, -cameraRotation.x);
            direction = rotateAroundYAxis(direction, -cameraRotation.y);
            direction = rotateAroundZAxis(direction, -cameraRotation.z);
            eye = eye + direction;
        }
    }

/*
    bool menu = true;
   
    while(menu == true){
        char key = getch();
        
        if(key == 0x1B) {
            menu = false;
        }
        else if(key == 'a'){
            eye = eye + Vector3f(-1,0,0);
            center = center + Vector3f(-1,0,0);
        }
        else if(key == 'd'){
            eye = eye + Vector3f(1,0,0);
            center = center + Vector3f(1,0,0);
        }
        else if(key == 'w'){
            eye = eye + Vector3f(0,0,-1);
            center = center + Vector3f(0,0,-1);
        }
        else if(key == 's'){
            eye = eye + Vector3f(0,0,1);
            center = center + Vector3f(0,0,1);
        }
        else if(key == 'q'){
            rotationY = (rotationY - 5) % 360;
        }
        else if(key == 'e'){
            rotationY = (rotationY + 5) % 360;
        }

        recalculate_transformation();

        //g_screen.reset_layer_polygons();
        //g_screen.draw_polygon_object(obj);
        //g_screen.draw_polygon_object(cube2);
        g_screen.draw_polygon_debug_data();
        g_screen.redraw();
    }
*/

    _setvideomode(_DEFAULTMODE);

    printf("exiting!\n");
    return 0;

}