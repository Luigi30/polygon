#include "widgets\button.hpp"
#include "framebuffer.hpp"

W_Button::W_Button(){};

W_Button::W_Button(std::string _name, Point _pos, ButtonShape _shape, Size2D _size){
    name = _name;
    position = Point(_pos.getX(), _pos.getY());
    shape = _shape;
    size = _size;
    text = "";
    isDown = false;
    isClickable = false;
};

W_Button::W_Button(std::string _name, Point _pos, ButtonShape _shape, Size2D _size, std::string _text){
    name = _name;
    position = Point(_pos.getX(), _pos.getY());
    shape = _shape;
    size = _size;
    text = _text;
    isDown = false;
    isClickable = false;
};

W_Button::W_Button(std::string _name, Point _pos, ButtonShape _shape, Size2D _size, std::string _text, bool _clickable) {
    name = _name;
    position = Point(_pos.getX(), _pos.getY());
    shape = _shape;
    size = _size;
    text = _text;
    isDown = false;
    isClickable = _clickable;
};

void W_Button::redraw(Framebuffer *layer, Framebuffer *layer_text){

    if(!isDown){
        //Draw a rectangle at position.
        //layer_text->putString("Drawing up button!  ", strlen("Drawing up button!  "), Point(0, 8), COLOR_WHITE, FONT_4x6);

        layer->draw_rectangle_filled(position, size.getX(), size.getY(), COLOR_LTGRAY);
        
        //shading: brighter in the upper and left sides
        layer->draw_line(position,
            Point(position.getX()+size.getX(), position.getY()),
            COLOR_WHITE);
        layer->draw_line(position,
            Point(position.getX(), position.getY()+size.getY()),
            COLOR_WHITE);

        //darker in the right and bottom sides
        layer->draw_line(Point(position.getX()+size.getX(), position.getY()),
            Point(position.getX()+size.getX(), position.getY()+size.getY()),
            COLOR_DKGRAY);
        layer->draw_line(Point(position.getX(), position.getY()+size.getY()),
            Point(position.getX()+size.getX(), position.getY()+size.getY()),
            COLOR_DKGRAY);

        //now draw the text
        layer_text->putString(text.c_str(), text.length(), Point(position.getX(), position.getY() + ((size.getY() - 6)/2)), COLOR_BLUE, FONT_4x6); 
    } else {
        //Button is down

        //Draw a rectangle at position.
        //layer_text->putString("Drawing down button!", strlen("Drawing down button!"), Point(0, 8), COLOR_WHITE, FONT_4x6);

        layer->draw_rectangle_filled(position, size.getX(), size.getY(), COLOR_DKGRAY);
        
        //shading: darker in the upper and left
        layer->draw_line(position,
            Point(position.getX()+size.getX(), position.getY()),
            COLOR_LTGRAY);
        layer->draw_line(position,
            Point(position.getX(), position.getY()+size.getY()),
            COLOR_LTGRAY);

        //lighter in the lower and right
        layer->draw_line(Point(position.getX()+size.getX(), position.getY()),
            Point(position.getX()+size.getX(), position.getY()+size.getY()),
            COLOR_WHITE);
        layer->draw_line(Point(position.getX(), position.getY()+size.getY()),
            Point(position.getX()+size.getX(), position.getY()+size.getY()),
            COLOR_WHITE);

        //now draw the text
        std::string spaces = "";
        for(int i=0;i<text.length();i++){
            spaces.append(" ");
        }
        
        layer_text->putString(spaces.c_str(), spaces.length(), Point(position.getX(), position.getY() + ((size.getY() - 6)/2)), COLOR_BLUE, FONT_4x6); 
        layer_text->putString(text.c_str(), text.length(), Point(position.getX()+1, position.getY()+1 + ((size.getY() - 6)/2)), COLOR_BLUE, FONT_4x6);
    }
    
    
}

int W_Button::pointIsInside(Point _point){
    int minX = position.getX() * 2;
    int maxX = minX + size.getX() * 2;

    int minY = position.getY();
    int maxY = minY + size.getY();

    if(_point.getX() >= minX && _point.getX() <= maxX && _point.getY() >= minY && _point.getY() <= maxY){
        return true;
    } else {
        return false;
    }
}

void W_Button::onClick() {
    //do nothing;
}
