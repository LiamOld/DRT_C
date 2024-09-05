//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_LINE2D_H
#define DRT_2D_VERSION3_LINE2D_H

#include <iostream>
#include <cmath>
#include "Point2D.h"
using namespace std;

class Line2D {
private:
    Point2D a, b;
    float A, B, C;
public:
    Line2D(const Point2D &a, const Point2D &b) : a(a), b(b) {
        A = b.getY() - a.getY();
        B = a.getX() - b.getX();
        C = A*a.getX() + B*a.getY();
    }

    float getA() const {
        return A;
    }

    float getB() const {
        return B;
    }

    float getC() const {
        return C;
    }

    Point2D geta() const {
        return a;
    }

    Point2D getb() const {
        return b;
    }
};


#endif //DRT_2D_VERSION3_LINE2D_H
