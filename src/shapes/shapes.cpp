#include "shapes\shapes.hpp"

Shape::Shape(char *_name, char *_rawPointText){
    strcpy(name, _name);

    /* Now do all that cool point stuff */
    int pointsFound = 0;

    int delimiterLocations[8];
    for(int i=0;i<strlen(_rawPointText);i++){
        if(_rawPointText[i] == '|'){
            delimiterLocations[pointsFound] = i;
            pointsFound++;
        }
    }
    pointsFound++;

    //There should be a , before each delimiter.
    //Point *points = new Point[8];
    int assignedPoints = 0;

    char *token;

    token = strtok(_rawPointText, "|");

    while(token != NULL) {
        //printf("%s : ", token);
        points[assignedPoints] = Point(token);
        assignedPoints++;
        token = strtok(NULL, "|");
    }

    numPoints = assignedPoints;
}