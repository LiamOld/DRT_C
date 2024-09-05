cmake_minimum_required(VERSION 3.27)
project(DRT_2D_version3)

set(CMAKE_CXX_STANDARD 17)

add_executable(DRT_2D_version3 main.cpp
        Vector2D.h
        Surface2D.h
        ReflectionPoint.h
        Point2D.h
        Path.h
        MultiPath.h
        Line2D.h)
