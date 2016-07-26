#include "screen.hpp"
#include <time.h>

int polygonCount = 0;

void Screen::redraw(){
    polygonCount = 0;
    clock_t start = clock();
    
    //render buttons to layer_background and text to layer_text
    memset(layer_widgets.pixels, 0, VGA_SIZE);
    /*
    for(int i=0;i<widgetsList.size();i++){
        widgetsList[i]->redraw(&layer_widgets, &layer_text);
    }
    */

    memset(layer_polygons.pixels, 0, VGA_SIZE);
    layer_polygons.reset_zbuffer();
    for(int i=0;i<sceneObjects.size();i++){
        draw_polygon_object(sceneObjects[i]);
    }
    
    memset(layer_final.pixels, 0, VGA_SIZE);    
    layer_final.overlay(layer_background, VGA_SIZE);
    layer_final.overlay(layer_polygons, VGA_SIZE);
    layer_final.overlay(layer_widgets, VGA_SIZE);
    layer_final.overlay(layer_text, VGA_SIZE);

    clock_t timing = clock() - start;
    char timingText[80];
    sprintf(timingText, "TICKS : %d", timing);
    layer_text.putString(timingText, strlen(timingText), Point(0,176), COLOR_GREEN, FONT_4x6);
    char fpsText[80];
    sprintf(fpsText, "FPS   : %f", (CLOCKS_PER_SEC / (float)timing));
    layer_text.putString(fpsText, strlen(fpsText), Point(0,184), COLOR_GREEN, FONT_4x6);

    //disable cursor while redrawing or we get graphic garbage on screen
    //Mouse::cursorDisable();
    memcpy(VGA_PTR, layer_final.pixels, VGA_SIZE);
    //Mouse::cursorEnable();
}

void Screen::init_framebuffers(){
    layer_background    = Framebuffer();
    layer_polygons      = Framebuffer();
    layer_widgets       = Framebuffer();
    layer_text          = Framebuffer();
    layer_final         = Framebuffer();    
}

void Screen::addButton(Button _button){
    buttons.push_back(_button);
}

void Screen::drawButton(Button _button){
    
//Draw a rectangle at position.
    //if(shape == BUTTON_SHAPE_RECT) {
        layer_widgets.draw_rectangle_filled(_button.position, _button.sizeX, _button.sizeY, COLOR_LTGRAY);
        
        //shading: brighter in the upper and left sides
        layer_widgets.draw_line(_button.position,
                Point(_button.position.getX()+_button.sizeX, _button.position.getY()),
                COLOR_WHITE);
        layer_widgets.draw_line(_button.position,
                Point(_button.position.getX(), _button.position.getY()+_button.sizeY),
                COLOR_WHITE);

        //darker in the right and bottom sides
        layer_widgets.draw_line(Point(_button.position.getX()+_button.sizeX, _button.position.getY()),
                Point(_button.position.getX()+_button.sizeX, _button.position.getY()+_button.sizeY),
                COLOR_DKGRAY);
        layer_widgets.draw_line(Point(_button.position.getX(), _button.position.getY()+_button.sizeY),
                Point(_button.position.getX()+_button.sizeX, _button.position.getY()+_button.sizeY),
                COLOR_DKGRAY);
    //}

    //now draw the text
    layer_text.putString(_button.text.c_str(), _button.text.length(), Point(_button.position.getX(), _button.position.getY() + ((_button.sizeY - 6)/2)), 0x10, FONT_4x6);
}

Widget* Screen::getClickedWidget(Point _point){
    //which widget did we click? -1 if none, widget ID otherwise
    for(int i=0;i<widgetsList.size();i++){
        if(widgetsList[i]->pointIsInside(_point) && widgetsList[i]->getIsClickable()){
            return widgetsList[i];
        }
    }
    return NULL;
}

std::string Screen::getClickedWidgetName(Point _point){
    Widget *clicked = getClickedWidget(_point);
    if(clicked != NULL){
        return clicked->getName();
    } else {
        return NULL;
    }
}

void Screen::removeWidget(std::string _name){
    //locate widget with the matching name and remove it from the list
    //widget names must be unique
    
    for(int i=0;i<widgetsList.size();i++){
        if(widgetsList[i]->getName() == _name){
            //remove text inside the widget area
            layer_text.clear_area(widgetsList[i]->getPosition(), widgetsList[i]->getSize());            
            widgetsList.erase(widgetsList.begin() + i);
            return;
        }
    }    
}

Widget* Screen::getWidgetByName(std::string _name){
    for(int i=0;i<widgetsList.size();i++){
        if(widgetsList[i]->getName() == _name){
            return widgetsList[i];
        }
    }

    return NULL;
}

/* Polygon routines */
void Screen::draw_polygon_object(SceneObject object){    
    for(int i=0;i<object.model.getFaceCount();i++){
        layer_polygons.draw_face(object, eye, cameraRotation, i) ? polygonCount++ : polygonCount;
    }
}

void Screen::draw_polygon_debug_data(){
    char eyeLocation[80];
    sprintf(eyeLocation, "EYE   : (%f, %f, %f)", eye.x, eye.y, eye.z);
    layer_text.putString(eyeLocation, strlen(eyeLocation), Point(0,0), COLOR_GREEN, FONT_4x6);

    char centerLocation[80];
    sprintf(centerLocation, "CAMROT: (%f, %f, %f)", cameraRotation.x, cameraRotation.y, cameraRotation.z);
    layer_text.putString(centerLocation, strlen(centerLocation), Point(0,8), COLOR_GREEN, FONT_4x6);

    char polycountLocation[80];
    sprintf(polycountLocation, "POLYS : %d", polygonCount);
    layer_text.putString(polycountLocation, strlen(polycountLocation), Point(0,192), COLOR_GREEN, FONT_4x6);
}

void Screen::draw_object_debug_data(SceneObject obj){
    char translationText[80];
    sprintf(translationText, "ORIGIN: (%f, %f, %f)", obj.transformation.translation.x, obj.transformation.translation.y, obj.transformation.translation.z);
    layer_text.putString(translationText, strlen(translationText), Point(0,16), COLOR_GREEN, FONT_4x6);

    char rotationText[80];
    sprintf(rotationText, "ROTATE: (%f, %f, %f)", obj.transformation.rotation.x, obj.transformation.rotation.y, obj.transformation.rotation.z);
    layer_text.putString(rotationText, strlen(rotationText), Point(0,24), COLOR_GREEN, FONT_4x6);

    char scaleText[80];
    sprintf(scaleText, "SCALE : (%f, %f, %f)", obj.transformation.scale.x, obj.transformation.scale.y, obj.transformation.scale.z);
    layer_text.putString(scaleText, strlen(scaleText), Point(0,32), COLOR_GREEN, FONT_4x6);
    
}

void Screen::addSceneObject(std::string _name, WavefrontObject _model, Vector3f _translation, Vector3f _rotation, Vector3f _scale){
    SceneObject obj;
    obj.name = _name;
    obj.model = _model;
    obj.transformation.translation = _translation;
    obj.transformation.rotation = _rotation;
    obj.transformation.scale = _scale;    
    sceneObjects.push_back(obj);
}

void Screen::removeSceneObject(std::string _name){
    for(int i=0;i<sceneObjects.size();i++){
        if(sceneObjects[i].name == _name){
            sceneObjects.erase(sceneObjects.begin() + i);
            return;
        }
    }

    //error: trying to delete object that doesn't exist
    assert(false);
    return;
}

SceneObject* Screen::getSceneObjectPtr(std::string _name){
    for(int i=0;i<sceneObjects.size();i++){
        if(sceneObjects[i].name == _name){
            return &sceneObjects[i];
        }
    }
    
    //no object found
    assert(false);
    return NULL;
}

void Screen::applyObjectVelocities(){
    for(int i=0;i<sceneObjects.size();i++){
        sceneObjects[i].apply_velocity_to_translation();
    }
}

void Screen::applyObjectRotations(){
    for(int i=0;i<sceneObjects.size();i++){
        sceneObjects[i].update_rotation();
    }
}