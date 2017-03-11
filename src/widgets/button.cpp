#include "raster\framebuffer.hpp"
#include "widgets\button.hpp"

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

        layer->draw_rectangle_filled(position, size, COLOR_LTGRAY);
        layer->shade_rectangle(position, size, COLOR_WHITE, COLOR_DKGRAY); //the light/dark contrast on the outline

        //draw string now
        layer_text->putString(text.c_str(), text.length(), Point(position.getX(), position.getY() + ((size.getY() - 6)/2)), COLOR_BLUE, FONT_4x6); 
    } else {
        //Button is down

        //Draw a rectangle at position.
        //layer_text->putString("Drawing down button!", strlen("Drawing down button!"), Point(0, 8), COLOR_WHITE, FONT_4x6);

        layer->draw_rectangle_filled(position, size, COLOR_DKGRAY);
        layer->shade_rectangle(position, size, COLOR_LTGRAY, COLOR_WHITE);

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
