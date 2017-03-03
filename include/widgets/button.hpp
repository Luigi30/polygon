#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "widget.hpp"

class W_Button : public Widget {
    ButtonShape shape;
    std::string text;
    
    public:
    W_Button();
    W_Button(std::string, Point, ButtonShape, Size2D);
    W_Button(std::string, Point, ButtonShape, Size2D, std::string);
    W_Button(std::string, Point, ButtonShape, Size2D, std::string, bool);
    void onClick();
    virtual void redraw(Framebuffer *background, Framebuffer *text);
    virtual int pointIsInside(Point _point);
    bool isDown;
};

#endif
