#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "colors.hpp"
#include "defines.hpp"
#include "fonts\font.hpp"
#include "shapes\shapes.hpp"

class Framebuffer;

#define W_CLICKABLE       true;
#define W_NOT_CLICKABLE   false;

class Widget {
    //An interactive control on the screen, i.e. button or something

    protected:
    Point position;
    Size2D size;
    COLOR color;
    std::string name;
    bool isClickable;

    public:
    virtual std::string getName();
    Point getPosition() { return position; };
    Size2D getSize() { return size; };
    bool getIsClickable() { return isClickable; };
    virtual void onClick(){};

    /* virtual methods */
    virtual void redraw(Framebuffer *background, Framebuffer *text) = 0;
    //virtual void remove(Framebuffer *background, Framebuffer *text) = 0;
    virtual int pointIsInside(Point);
};

#endif
