//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_POINT2D_H
#define DRT_2D_VERSION3_POINT2D_H

#include <iostream>
#include <cmath>
#include "Vector2D.h"
using namespace std;

class Point2D {
private:
    float x,y;
    Vector2D speed;
    Vector2D acceleration;
public:
    void update(float t){
        x += t*speed.x + t*t*acceleration.x/2 ;
        y += t*speed.y + t*t*acceleration.y/2;
        speed = speed + acceleration*t;
    }

    Point2D(float x,
            float y,
            Vector2D speed=Vector2D(0,0),
            Vector2D acceleration=Vector2D(0,0) ):
            x(x), y(y), speed(speed), acceleration(acceleration){}

    // point a-b, return a vector from b to a;
    Vector2D operator-(const Point2D& p)const{
        return {x-p.x, y-p.y};
    }

    // a+v, means translate point a by vector v
    Point2D operator+(const Vector2D v)const{
        return {x+v.x, y+v.y, speed, acceleration};
    }

    void display() const{
        cout << "x= " << x << "; y= " << y << endl;
    }

    Vector2D getSpeed() const{
        return speed;
    }

    Vector2D getAcceleration() const {
        return acceleration;
    }

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }

    void setX(float x) {
        Point2D::x = x;
    }

    void setY(float y) {
        Point2D::y = y;
    }

    void setAcceleration(const Vector2D &acceleration) {
        Point2D::acceleration = acceleration;
    }

    void setSpeed(const Vector2D &speed) {
        Point2D::speed = speed;
    }
};


#endif //DRT_2D_VERSION3_POINT2D_H
