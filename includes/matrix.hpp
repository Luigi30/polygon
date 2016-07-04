#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <math.h>
#include <cassert>
#include <vector>
#include <cstdio>
#include "..\shapes\vector3f.hpp"
#include "..\shapes\point.hpp"

#define _X 0
#define _Y 1
#define _Z 2
#define _W 3

#define M_PI 3.141592653589793238462643383279502884

class Matrix {
    std::vector< std::vector<float> > data;
    int rows, cols;

    public:
    Matrix(){}
    
    Matrix(int _rows, int _cols) {
        rows = _rows;
        cols = _cols;
        
        for(int i=0; i<rows; i++){
            data.push_back(std::vector<float>()); //each row contains an float vector
            for(int j=0; j<cols; j++){
                data[i].push_back(0.0); //and each column is initialized to 0.0f
            }
        }
    }

    static Matrix identity(int dimensions){
        Matrix m = Matrix(dimensions, dimensions);

        for(int i=0;i<dimensions;i++){
            for(int j=0;j<dimensions;j++){
                m.data[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }

        return m;
    }

    inline int numRows() { return rows; };
    int numColumns() { return cols; };

    std::vector<float>& operator[](const int row){
        return data[row];
    }

    //Matrix multiplied by a vector of arbitrary rows
    Matrix operator*(Matrix m) {
        assert(rows == m.numRows() && (cols == m.numColumns() || m.cols == 1));

        //Matrix * Matrix
        Matrix result = Matrix(rows, m.cols);

        for(int rightRow = 0; rightRow < m.numRows(); rightRow++){
            for(int r=0; r<rows; r++){
                for(int c=0; c<cols; c++){
                    result[r][rightRow] += data[r][c] * m[c][rightRow];
                }
            }
        }

        return result;
    }

    Matrix operator+(Matrix m){
        assert(rows == m.numRows() && cols == m.numColumns());

        Matrix result = Matrix(rows, cols);

        for(int r=0;r<rows;r++){
            for(int c=0;c<cols;c++){
                result[r][c] = data[r][c] + m[r][c];
            }
        }
        return result;
    }

    Matrix operator-(Matrix m){
        assert(rows == m.numRows() && cols == m.numColumns());

        Matrix result = Matrix(rows, cols);

        for(int r=0;r<rows;r++){
            for(int c=0;c<cols;c++){
                result[r][c] = data[r][c] - m[r][c];
            }
        }
        return result;
    }

    static Matrix Matrix::translation(float x, float y, float z){
        Matrix m = identity(4);
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
        return m;
    }

    static Matrix Matrix::scale(float x, float y, float z){
        Matrix m = identity(4);
        m[0][0] = x;
        m[1][1] = y;
        m[2][2] = z;
        return m;
    }

    //rotation specified in degrees, translate to radians
    static Matrix Matrix::rotation(float x, float y, float z){
        float x_radians = (x * M_PI) / 180.0;
        float y_radians = (y * M_PI) / 180.0;
        float z_radians = (z * M_PI) / 180.0;        
        
        #define COSX std::cos(x_radians)
        #define COSY std::cos(y_radians)
        #define COSZ std::cos(z_radians)
        #define SINX std::sin(x_radians)
        #define SINY std::sin(y_radians)
        #define SINZ std::sin(z_radians)
        
        Matrix result = Matrix::identity(4);

        result[0][0] = (COSY * COSZ);
        result[0][1] = (COSZ * SINX * SINY) - (COSX * SINZ);
        result[0][2] = (COSX * COSZ * SINY) + (SINX * SINZ);
        result[0][3] = 0;

        result[1][0] = (COSY * SINZ);
        result[1][1] = (COSX * COSZ) + (SINX * SINY * SINZ);
        result[1][2] = -(COSZ * SINX) + (COSX * SINY * SINZ);
        result[1][3] = 0;

        result[2][0] = -(SINY);
        result[2][1] = (COSY * SINX);
        result[2][2] = (COSX * COSY);
        result[2][3] = 0;

        result[3][0] = 0;
        result[3][1] = 0;
        result[3][2] = 0;
        result[3][3] = 1;
        
        return result;
    }


    /* Vector behavior */
    static Matrix Matrix::vector(float x){
        Matrix m = Matrix(1,1);
        m[0][0] = x;
        return m;
    }

    static Matrix Matrix::vector(float x, float y){
        Matrix m = Matrix(2,1);
        m[0][0] = x;
        m[1][0] = y;
        return m;
    }

    static Matrix Matrix::vector(float x, float y, float z){
        Matrix m = Matrix(3,1);
        m[0][0] = x;
        m[1][0] = y;
        m[2][0] = z;
        return m;
    }
    
    static Matrix Matrix::vector(float x, float y, float z, float w){
        Matrix m = Matrix(4,1);
        m[0][0] = x;
        m[1][0] = y;
        m[2][0] = z;
        m[3][0] = w;
        return m;
    }

    float X(){
        return data[0][0];
    }

    float Y(){
        return data[1][0];
    }

    float Z(){
        return data[2][0];
    }

    float W(){
        return data[3][0];
    }

    Matrix transpose(){
        Matrix result(cols, rows);

        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                result[j][i] = data[i][j];
            }
        }

        return result;
    }

    Matrix Matrix::inverse() {
        assert(rows==cols);
        // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
        Matrix result(rows, cols*2);
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                result[i][j] = data[i][j];
        for(int i=0; i<rows; i++)
            result[i][i+cols] = 1;

        // first pass
        for (int i=0; i<rows-1; i++) {
            // normalize the first row
            for(int j=result.cols-1; j>=0; j--)
                result[i][j] /= result[i][i];
            for (int k=i+1; k<rows; k++) {
                float coeff = result[k][i];
                for (int j=0; j<result.cols; j++) {
                    result[k][j] -= result[i][j]*coeff;
                }
            }
        }
        // normalize the last row
        for(int j=result.cols-1; j>=rows-1; j--)
            result[rows-1][j] /= result[rows-1][rows-1];
            
        // second pass
        for (int i=rows-1; i>0; i--) {
            for (int k=i-1; k>=0; k--) {
                float coeff = result[k][i];
                for (int j=0; j<result.cols; j++) {
                    result[k][j] -= result[i][j]*coeff;
                }
            }
        }
        
        // cut the identity matrix back
        Matrix truncate(rows, cols);
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                truncate[i][j] = result[i][j+cols];
        return truncate;
    }

    void print(){
        printf("rows: %d, cols: %d\n", rows, cols);
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                printf("%f ", data[i][j]);
            }
            printf("\n");
        }
    }

    static Matrix Matrix::position_vector(Vector3f v){
        Matrix m = Matrix(4,1);
        m[0][0] = v.x;
        m[1][0] = v.y;
        m[2][0] = v.z;
        m[3][0] = 1;
        return m;
    }

    static Matrix Matrix::direction_vector(Vector3f v){
        Matrix m = Matrix(4,1);
        m[0][0] = v.x;
        m[1][0] = v.y;
        m[2][0] = v.z;
        m[3][0] = 0;
        return m;
    }

    static Vector3f transform_3d(Matrix translation, Matrix rotation, Matrix scale, Matrix coordinates){
        assert(translation.numRows() == 4 && translation.numColumns() == 4);
        assert(rotation.numRows() == 4 && rotation.numColumns() == 4);
        assert(scale.numRows() == 4 && scale.numColumns() == 4);
        assert(coordinates.numRows() == 4 && coordinates.numColumns() == 1);

        Matrix transformed = translation * rotation * scale * coordinates;

        return Vector3f(transformed[0][0], transformed[1][0], transformed[2][0]);
    }

    static Vector3f perspective_projection(Matrix coordinates, float camera_distance_from_origin){
        float c = camera_distance_from_origin;
        assert(coordinates.numRows() == 4 && coordinates.numColumns() == 1);

        Matrix perspective = identity(4);
        perspective[3][2] = (-1.0/camera_distance_from_origin);
        Matrix result = perspective * coordinates;

        return Vector3f((result.X()/(1-result.Z()/c)),
                        (result.Y()/(1-result.Z()/c)),
                        (result.Z()/(1-result.Z()/c)));

/*
        return Vector3f((int)((result.X()/(1-result.Z()/c)+.5)),
                        (int)((result.Y()/(1-result.Z()/c)+.5)),
                        (result.Z()/(1-result.Z()/c)));
*/
    }
};

#endif