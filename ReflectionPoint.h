//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_REFLECTIONPOINT_H
#define DRT_2D_VERSION3_REFLECTIONPOINT_H

#include "Point2D.h"

class ReflectionPoint {
public:
    Point2D point;
    float life;

    ReflectionPoint(const Point2D &point, float life) : point(point), life(life) {}

    void update(float t){
        point.update(t);
        life -= t;
    }
};


#endif //DRT_2D_VERSION3_REFLECTIONPOINT_H
