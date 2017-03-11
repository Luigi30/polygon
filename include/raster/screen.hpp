#pragma once

#include <vector>
#include <stdlib.h>
#include "framebuffer.hpp"
#include "rasterize.hpp"
#include "widgets\widget.hpp"
#include "objects\scenemgr.hpp"

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
    //std::vector<SceneObject> sceneObjects;
    
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
