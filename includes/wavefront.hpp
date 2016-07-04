/* Loads Wavefront OBJ files */

#ifndef WAVEFRONT_HPP
#define WAVEFRONT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "..\shapes\point3d.hpp"
#include "..\includes\matrix.hpp"

#define MAX_VERTICES 4096
#define MAX_FACES 4096

struct Face {
    int v1;
    int v2;
    int v3;
    
    Face(){};
    
    Face(int a, int b, int c){
        v1 = a;
        v2 = b;
        v3 = c;
    }
};

/* TODO: this is a model class */
class WavefrontObject {
    Vector3f *vertices;
    Vector3f *worldVertices;
    Face *faces;
    int vertexCount;
    int faceCount;
    void process_line(char *line);
    
    public:
    bool load_file(std::string filename);
    Vector3f* getLocalVertices() { return vertices; };
    Face* getFaces() { return faces; };

    int getVertexCount() { return vertexCount-1; };
    int getFaceCount() { return faceCount; };

    Vector3f translation;
    Vector3f rotation;
    Vector3f scale;
    Matrix getModelMatrix() {
        return (Matrix::translation(translation.x, translation.y, translation.z) *
                Matrix::rotation(rotation.x, rotation.y, rotation.z) *
                Matrix::scale(scale.x, scale.y, scale.z));
    }
        
    Vector3f* getWorldVertices(){
        return worldVertices;
    }
};

void WavefrontObject::process_line(char *line){
//Vertex
        if(line[0] == 'v' && line[1] == ' '){
            //printf("%s", line);
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            char type[4];
            
            sscanf(line, "%s %f %f %f", type, &x, &y, &z);            
            vertices[vertexCount] = Vector3f(x, y, z);
            
            //printf("vertex is %f %f %f\n", vertices[vertexCount].x, vertices[vertexCount].y, vertices[vertexCount].z);
            vertexCount++;
        }

        //Triangle
        if(line[0] == 'f'){
            int geometricVertex1 = 0;
            int geometricVertex2 = 0;
            int geometricVertex3 = 0;

            int textureVertex1 = 0;
            int textureVertex2 = 0;
            int textureVertex3 = 0;

            int normalVertex1 = 0;
            int normalVertex2 = 0;
            int normalVertex3 = 0;

            char type[4];

            //detect which format the line is in
            if(strchr(line, '/') == NULL){
                sscanf(line, "%s %d %d %d", type, &geometricVertex1, &geometricVertex2, &geometricVertex3);
            } else {
                sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d", type, &geometricVertex1, &textureVertex1, &normalVertex1, &geometricVertex2, &textureVertex2, &normalVertex2, &geometricVertex3, &textureVertex3, &normalVertex3);
            }
            faces[faceCount] = Face(geometricVertex1, geometricVertex2, geometricVertex3);
            faceCount++;
        }
}

bool WavefrontObject::load_file(std::string filename){
    FILE *objectFile;

    vertexCount = 1; //vertices start at 1
    faceCount = 0;

    vertices = (Vector3f*)malloc(sizeof(Vector3f) * MAX_VERTICES);
    if(vertices == NULL){
        printf("malloc() failed for vertex list\n");
        exit(0);
    }

    worldVertices = (Vector3f*)malloc(sizeof(Vector3f) * MAX_VERTICES);
    if(worldVertices == NULL){
        printf("malloc() failed for world vertex list\n");
        exit(0);
    }
    
    faces = (Face*)malloc(sizeof(Face) * MAX_VERTICES);
    if(faces == NULL){
        printf("malloc() failed for face list\n");
        exit(0);
    }

    objectFile = fopen(filename.c_str(), "r");
    if(objectFile == NULL){
        printf("Failed to load file %s", filename);
        exit(0);
    }
    
    int lineCount = 0;
    char c;

    char line_chars[100];
    std::vector<std::string> lines;
    
    std::ifstream obj(filename.c_str());
    if(obj.is_open()){
        int length = 0;
        while(obj.get(c)){
            if(c == '\n'){
                length = 0;
                printf("%s\n", line_chars);
                process_line(line_chars);
                memset(line_chars, 0, 100);
            } else {
                line_chars[length] = c;
                length++;
            }
        }
        //we should have one more line in the buffer
        printf("%s\n", line_chars);
        process_line(line_chars);
    }

    fclose(objectFile);

    translation = Vector3f(0,0,0); //Matrix::translation(0,0,0);
    rotation    = Vector3f(0,0,0); //Matrix::identity(4);
    scale       = Vector3f(1,1,1); //Matrix::scale(1,1,1);
    return true;
}

#endif