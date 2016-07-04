#include "main.hpp"

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

    center = center + Vector3f(0,0,0);

    WavefrontObject obj;
    obj.load_file("cube.3d");
    g_screen.addSceneObject("cube1", obj);

    WavefrontObject cube2;
    cube2.load_file("cube.3d");
    cube2.translation = Vector3f(1,0,0);
    cube2.scale = Vector3f(.5,.5,.5);
    g_screen.addSceneObject("cube2", cube2);
    
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
    //g_screen.draw_object_debug_data(obj);
    g_screen.redraw();

    bool menu = true;
    
    while(menu == true){
        char key = getch();
        
        if(key == 0x1B) {
            menu = false;
        }
        else if(key == 'a'){
            eye = eye + Vector3f(-1,0,0);
        }
        else if(key == 'd'){
            eye = eye + Vector3f(1,0,0);
        }
        else if(key == 'w'){
            eye = eye + Vector3f(0,1,0);

        }
        else if(key == 's'){
            eye = eye + Vector3f(0,-1,0);
        }

        else if(key == 'A'){
            center = center - Vector3f(0,.1,0);
        }
        else if(key == 'D'){
            center = center + Vector3f(0,.1,0);
        }
        else if(key == 'W'){
            center = center - Vector3f(0,0,.1);
        }
        else if(key == 'S'){
            center = center + Vector3f(0,0,.1);
        }
        
        else if(key == 'j'){
            obj.translation = obj.translation + Vector3f(-1,0,0);
        }
        else if(key == 'l'){
            obj.translation = obj.translation + Vector3f(1,0,0);
        }
        else if(key == 'i'){
            obj.translation = obj.translation + Vector3f(0,1,0);
        }
        else if(key == 'k'){
            obj.translation = obj.translation + Vector3f(0,-1,0);
        }

        recalculate_transformation();

        //g_screen.reset_layer_polygons();
        //g_screen.draw_polygon_object(obj);
        //g_screen.draw_polygon_object(cube2);
        g_screen.draw_polygon_debug_data();
        g_screen.redraw();
    }

    _setvideomode(_DEFAULTMODE);

    printf("exiting!\n");
    return 0;

}