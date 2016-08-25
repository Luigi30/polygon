/* Loads Wavefront OBJ files */

#ifndef WAVEFRONT_HPP
#define WAVEFRONT_HPP

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "..\shapes\vector3f.hpp"
#include "..\shapes\point3d.hpp"

#define MAX_VERTICES 512
#define MAX_FACES 512

struct Face {
    //Geometry vertices
    int v1;
    int v2;
    int v3;

    //Texture vertices
    int t1;
    int t2;
    int t3;
    
    Face(){};
    
    //No UV coordinates
    Face(int a, int b, int c){
        v1 = a;
        v2 = b;
        v3 = c;
        t1 = 0;
        t2 = 0;
        t3 = 0;
    }

    //With UV coordinates
    Face(int va, int vb, int vc, int ta, int tb, int tc){
        v1 = va;
        v2 = vb;
        v3 = vc;
        t1 = ta;
        t2 = tb;
        t3 = tc;
    }
};

/* TODO: this is a model class */
class WavefrontObject {
    Vector3f *geometryVertices;
    Vector3f *textureVertices;
    Vector3f *worldVertices;
    Face *faces;
    int vertexCount;
    int textureCoordinateCount;
    int faceCount;
    void process_line(char *line);
    
    public:
    bool load_file(std::string filename);
    Vector3f* getLocalVertices() { return geometryVertices; };
    Vector3f* getTextureVertices() { return textureVertices; };
    Face* getFaces() { return faces; };

    int getVertexCount() { return vertexCount-1; };
    int getFaceCount() { return faceCount; };
        
    Vector3f* getWorldVertices(){
        return worldVertices;
    }

    WavefrontObject(){};

    WavefrontObject(std::string filename){
        load_file(filename);
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
            geometryVertices[vertexCount] = Vector3f(x, y, z);
            
            //printf("vertex is %f %f %f\n", vertices[vertexCount].x, vertices[vertexCount].y, vertices[vertexCount].z);
            vertexCount++;
        }

        //Texture coordinates
        else if(line[0] == 'v' && line[1] == 't'){
            float u = 0.0f;
            float v = 0.0f;
            float w = 1.0f;
            char type[4];

            sscanf(line, "%s %f %f", type, &u, &v);
            textureVertices[textureCoordinateCount] = Vector3f(u, v, w);

            //_setvideomode(_DEFAULTMODE);
            printf("v0 U,V,W: %f,%f,%f\n", u, v, w);
            //exit(0);

            textureCoordinateCount++;
        }

        //Triangle
        else if(line[0] == 'f'){
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
                faces[faceCount] = Face(geometricVertex1, geometricVertex2, geometricVertex3);
            } else {
                sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d", type, &geometricVertex1, &textureVertex1, &normalVertex1, &geometricVertex2, &textureVertex2, &normalVertex2, &geometricVertex3, &textureVertex3, &normalVertex3);
                faces[faceCount] = Face(geometricVertex1, geometricVertex2, geometricVertex3, textureVertex1, textureVertex2, textureVertex3);
            }

            faceCount++;
        }
}

bool WavefrontObject::load_file(std::string filename){
    FILE *objectFile;

    vertexCount = 1; //vertices start at 1
    textureCoordinateCount = 1; //vertices start at 1
    faceCount = 0;

    geometryVertices = (Vector3f*)malloc(sizeof(Vector3f) * MAX_VERTICES);
    if(geometryVertices == NULL){
        printf("malloc() failed for geometry vertex list\n");
        exit(0);
    }

    textureVertices = (Vector3f*)malloc(sizeof(Vector3f) * MAX_VERTICES);
    if(textureVertices == NULL){
        printf("malloc() failed for texture vertex list\n");
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
    return true;
}

#endif