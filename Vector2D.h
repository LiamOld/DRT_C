//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_VECTOR2D_H
#define DRT_2D_VERSION3_VECTOR2D_H

#include "iostream"
#include "cmath"
using namespace std;

class Vector2D {
public:
    float x, y;
    Vector2D(float x, float y): x(x), y(y){}
    Vector2D operator+(const Vector2D& v)const{
        return {x+v.x, y+v.y};
    }
    Vector2D operator-(const Vector2D& v)const{
        return {x-v.x, y-v.y};
    }
    float dot(Vector2D v) const{
        return x * v.x + y * v.y;
    }
    float magnitude() const {
        return sqrt(x*x + y*y);
    }
    float magnitude_sql() const {
        return (x*x + y*y);
    }
    Vector2D normalize() const{
        float mag = magnitude();
        return mag > 0 ? Vector2D(x / mag, y / mag) : Vector2D(0, 0);
        // Return zero vector if original vector is zero
    }
    Vector2D operator*(const float s) const{
        return {x*s, y*s};
    }
    void display() {
        cout << "x= " << x << "; y= " << y << endl;
    };

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }
};


#endif //DRT_2D_VERSION3_VECTOR2D_H
