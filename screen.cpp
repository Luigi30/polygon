#include "screen.hpp"

void Screen::redraw(){
    //render buttons to layer_background and text to layer_text
    memset(layer_widgets.getPixels(), 0, VGA_SIZE);
    /*
    for(int i=0;i<widgetsList.size();i++){
        widgetsList[i]->redraw(&layer_widgets, &layer_text);
    }
    */

    memset(layer_polygons.getPixels(), 0, VGA_SIZE);
    layer_polygons.reset_zbuffer();
    for(int i=0;i<sceneObjects.size();i++){
        draw_polygon_object(sceneObjects[i]);
    }
    
    memset(layer_final.getPixels(), 0, VGA_SIZE);    
    layer_final.overlay(layer_background, VGA_SIZE);
    layer_final.overlay(layer_polygons, VGA_SIZE);
    layer_final.overlay(layer_widgets, VGA_SIZE);
    layer_final.overlay(layer_text, VGA_SIZE);

    //disable cursor while redrawing or we get graphic garbage on screen
    //Mouse::cursorDisable();
    memcpy(VGA_PTR, layer_final.getPixels(), VGA_SIZE);
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
void Screen::draw_polygon_object(SceneObject sceneObject){
    draw_polygon_object(sceneObject.model);
}

void Screen::draw_polygon_object(WavefrontObject object){    
    Matrix Model = object.getModelMatrix(); //local -> world transform matrix
    Matrix modelView = View * Model;
    
    for(int i=0;i<object.getFaceCount();i++){
        layer_polygons.draw_face(object, Viewport, Projection, modelView, i);
    }
}

void Screen::draw_polygon_debug_data(){
    char eyeLocation[80];
    sprintf(eyeLocation, "EYE   : (%f, %f, %f)", eye.x, eye.y, eye.z);
    layer_text.putString(eyeLocation, strlen(eyeLocation), Point(0,0), COLOR_GREEN, FONT_4x6);

    char centerLocation[80];
    sprintf(centerLocation, "CENTER: (%f, %f, %f)", center.x, center.y, center.z);
    layer_text.putString(centerLocation, strlen(centerLocation), Point(0,8), COLOR_GREEN, FONT_4x6);
}

void Screen::draw_object_debug_data(WavefrontObject model){
    char translationText[80];
    sprintf(translationText, "ORIGIN: (%f, %f, %f)", model.translation.x, model.translation.y, model.translation.z);
    layer_text.putString(translationText, strlen(translationText), Point(0,16), COLOR_GREEN, FONT_4x6);

    char rotationText[80];
    sprintf(rotationText, "ROTATE: (%f, %f, %f)", model.rotation.x, model.rotation.y, model.rotation.z);
    layer_text.putString(rotationText, strlen(rotationText), Point(0,24), COLOR_GREEN, FONT_4x6);

    char scaleText[80];
    sprintf(scaleText, "SCALE : (%f, %f, %f)", model.scale.x, model.scale.y, model.scale.z);
    layer_text.putString(scaleText, strlen(scaleText), Point(0,32), COLOR_GREEN, FONT_4x6);
    
}

void Screen::addSceneObject(std::string _name, WavefrontObject _model){
    SceneObject obj;
    obj.name = _name;
    obj.model = _model;
    sceneObjects.push_back(obj);
}

void Screen::removeSceneObject(std::string _name){
    for(int i=0;i<sceneObjects.size();i++){
        if(sceneObjects[i].name == _name){
            sceneObjects.erase(sceneObjects.begin() + i);
            return;
        }
    }
}