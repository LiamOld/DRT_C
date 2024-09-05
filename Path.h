//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_PATH_H
#define DRT_2D_VERSION3_PATH_H

#include "Vector2D.h"
#include "Surface2D.h"
#include "ReflectionPoint.h"

class Path {
public:
    ReflectionPoint reflectionPoint;
    int surface_index;

    Path(const ReflectionPoint &reflectionPoint, int surfaceIndex) : reflectionPoint(reflectionPoint),
                                                                     surface_index(surfaceIndex) {}

    void update(float t){ reflectionPoint.update(t);}
};


#endif //DRT_2D_VERSION3_PATH_H
