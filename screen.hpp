#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <vector>
#include <stdlib.h>
//#include "includes\mouse.hpp"
#include "framebuffer.hpp"
#include "rasterize.hpp"
#include "widgets\widget.hpp"
#include "objects\sceneobject.hpp"

#define GAMEOBJECT(X) g_screen.getSceneObjectPtr(X)

struct Button {
    Point position;
    int sizeX;
    int sizeY;
    std::string text;

    Button(){};

    Button(Point _pos, int _x, int _y, std::string _text){
        position = Point(_pos.getX(), _pos.getY());
        sizeX = _x;
        sizeY = _y;
        text = _text;
    }
};

class Screen {
    std::vector<Button> buttons;
    std::vector<SceneObject> sceneObjects;
    
    public:
    Framebuffer layer_background;
    Framebuffer layer_polygons;
    Framebuffer layer_widgets;
    Framebuffer layer_text;
    Framebuffer layer_final;

    void reset_layer_text() { layer_text.clear_area(Point(0,0), Size2D(320,200)); };
    void reset_layer_polygons() { layer_polygons.clear_area(Point(0,0), Size2D(320,200)); };

    void drawMouseCursor(Framebuffer layer);
    Widget* getWidgetByName(std::string _name);
    Widget* getClickedWidget(Point);
    std::string getClickedWidgetName(Point);

    std::vector<Widget*> widgetsList;
    void removeWidget(std::string _name);

    void addSceneObject(std::string _name, WavefrontObject _model, Vector3f _translation, Vector3f _rotation, Vector3f _scale);
    void removeSceneObject(std::string _name);
    SceneObject *getSceneObjectPtr(std::string _name);
    
    void Screen::applyObjectVelocities();
    void Screen::applyObjectRotations();

    void addButton(Button);
    void drawButton(Button);

    void init_framebuffers();
    void redraw();

    void draw_polygon_object(SceneObject sceneObject);
    void draw_polygon_debug_data();
    void draw_object_debug_data(SceneObject obj);

    void mode7_background(int angle_degrees, float scale_factor);
};

Screen g_screen;

#endif
