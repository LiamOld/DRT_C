#include <iostream>
#include <vector>
#include "Point2D.h"
#include "Surface2D.h"
#include "Vector2D.h"
#include "MultiPath.h"
#include "Path.h"
#include "ReflectionPoint.h"
using namespace std;

void test0(){
    //test the calculation of v and a in local coordinate
    Vector2D v_t(1,0), v_r(1,0);
    Point2D t(7,5, v_t), r(4,5, v_r);
    float v = Surface2D::velocity_reflection_local(t, r);
    cout << "v: "<<v << endl;

    float a = Surface2D::acceleration_reflection_local(t,r);
    cout << "a: " << a << endl;
}

void test1(){
    //test the coordinate, v and a in global coordinate
//    Vector2D v_f(0,1);
//    Point2D a(1,1, v_f), b(1,5, v_f);
//    Surface2D f(a,b);
//
//    Vector2D v(0,3);
//    Point2D t(2,1, v), r(2,5,v);

    Vector2D v_f(0,0);
    Point2D a(1,1, v_f), b(5,5, v_f);
    Surface2D f(a,b);

    Vector2D v(0,1);
    Point2D t(2,0, v), r(6,4,v);

    ReflectionPoint p = f.reflection_point(t,r);
    Point2D reflectionPoint = p.point;

    cout << "The coordinate of the reflection point: " << endl;
    reflectionPoint.display();

    Vector2D v_reflection = reflectionPoint.getSpeed();
    Vector2D a_reflection = reflectionPoint.getAcceleration();

    cout << "The speed of the reflection point: " << endl;
    v_reflection.display();
    cout << "The acceleration of the reflection point: " << endl;
    a_reflection.display();
}

// test get_life in surface class
void test2(){
    float life1 = Surface2D::get_life(1,1,2,9.5);
    cout << "life1: " << life1 << endl;
    float life2 = Surface2D::get_life(-1,-1,4,20);
    cout << "life2: " << life2 << endl;
    float life3 = Surface2D::get_life(5,-1,4,16);
    cout << "life3: " << life3 << endl;
    float life4 = Surface2D::get_life(-5,1,12,20);
    cout << "life4: " << life4 << endl;
    float life5 = Surface2D::get_life(0.65,-0.023,15,90);
    cout << "life5: " << life5 << endl;
}

// test the get_image() in surface class
void test3(){
    Vector2D v(-2,-2);
    Point2D p(5,5, v);
    Point2D a(0,0), b(0,1);
    Surface2D s(a,b);
    Point2D image = s.get_image(p);
    image.display();
    image.getSpeed().display();
}

// comparing the DRT and traditional RT
void test4(){
    // input: transmitter and receiver
    // Vector2D v_transmitter(0, 0), v_receiver(0,0);
    //velocity: m/s
    Vector2D v_transmitter(0, 15), v_receiver(0,10);
    Point2D transmitter(5,10, v_transmitter);
    Point2D receiver(10, 30, v_receiver);

    Surface2D wall = Surface2D(Point2D(0,10),Point2D(-5,50));
    vector<Surface2D> walls ={};
    walls.push_back(wall);
    MultiPath model(walls, transmitter, receiver);

//    for(Path path:model.getReflectionOrder1Valid()){
//        cout << "Order1:"<<endl;
//        path.reflectionPoint.point.display();
//        cout << "velocity: ";
//        path.reflectionPoint.point.getSpeed().display();
//        cout << "acceleration: ";
//        path.reflectionPoint.point.getAcceleration().display();
//        cout<< "life: "<<path.reflectionPoint.life<<endl;
//    }

    vector<float> x_DRT={}, y_DRT={}, x_RT={}, y_RT={};
    float time = 0;
    float t=0.1; // 0.1s for each update
    while (time<24){

//        cout << "time: " <<  time << endl;

        model.update(t);
        transmitter.update(t);
        receiver.update(t);
        wall.update(t);

//        cout << "Order1/by DRT:"<<endl;
        Point2D point_DRT = model.getReflectionOrder1Valid()[0].reflectionPoint.point;
//        point_DRT.display();
        x_DRT.push_back(point_DRT.getX());
        y_DRT.push_back(point_DRT.getY());

//        cout << "Order1/by Traditional RT:"<<endl;
        Point2D point_RT = wall.reflection_point(transmitter, receiver).point;
//        point_RT.display();
        x_RT.push_back(point_RT.getX());
        y_RT.push_back(point_RT.getY());

        time ++;
    }

    cout<<"x_DRT = [";
    for(auto value: x_DRT){
        cout << value << ", ";
    }
    cout<< endl;

    cout<<"y_DRT = [";
    for(auto value: y_DRT){
        cout << value << ", ";
    }
    cout<< endl;

    cout<<"x_RT = [";
    for(auto value: x_RT){
        cout << value << ", ";
    }
    cout<< endl;

    cout<<"y_RT = [";
    for(auto value: y_RT){
        cout << value << ", ";
    }
    cout<< endl;

}

//test the simplest model, two long surfaces.
void test5(){
    Point2D a1(0,0), b1(0, 100);
    Point2D a2(20,0), b2(20,100);
    Surface2D f1(a1,b1), f2(a2, b2);
    vector<Surface2D> fs = {f1, f2};

    Point2D t(10,0),r(10,50);

    MultiPath model(fs, t, r);
    cout << "number of order1 path: " << model.getReflectionOrder1Valid().size() << endl;
    cout << "number of order2 path: " << model.getReflectionOrder2Valid().size() << endl;

    if(model.getReflectionOrder1Valid().empty()){
        cout << "path_reflect_order1 : None" << endl;
    } else{
        cout << "path_reflect_order1 :" << endl;
        int n = 0;
        for(auto path : model.getReflectionOrder1Valid()){
            cout << "path" << n << ": " <<endl;
            n++;
            path.reflectionPoint.point.display();
        }
    }

    if(model.getReflectionOrder2Valid().empty()){
        cout << "path_reflect_order2 : None" << endl;
    } else{
        cout << "path_reflect_order2 :" << endl;
        int n=0;
        for(auto  paths: model.getReflectionOrder2Valid()){
            cout << "paths" << n << ": " <<endl;
            n++;
            cout<< "Reflection Point 1: " << endl;
            paths[0].reflectionPoint.point.display();
            cout<< "Reflection Point 2: " << endl;
            paths[1].reflectionPoint.point.display();
        }
    }
}


//input is 4 vertex of a rectangle, the output is 4 surfaces of this rectangle.
void add_surfaces(vector<Surface2D>& surfaces, const vector<Point2D>& points) {
    bool moving = !(points[0].getSpeed().x == 0 && points[0].getSpeed().y == 0);
    for (int i = 0; i < 4; i++) {
        // Connect each point with the next, and the last with the first
        surfaces.push_back(Surface2D(points[i], points[(i + 1) % 4]));
    }
}

// the final result
void test6(){
    // the test is in a road 30x300 meter, with buildings beside of the road
    // the buildings are about 20x10, and 10 buildings on each side.
    // 1: input: buildings surfaces
    vector<Surface2D> surfaces ={};
    // four points representing the bottom buildings on the left and right sides
    vector<Point2D> a1 = {Point2D(10,10), Point2D(10, 30), Point2D(20, 30), Point2D(20,10)};
    vector<Point2D> a2 = {Point2D(50,10), Point2D(50, 30), Point2D(60, 30), Point2D(60,10)};
    // There are 20 buildings in this test, and a loop is used to generate each building and add its surface to the surface array.
    // Each building is the same size and the distance between each building is 10 meters.
    for(int i=0; i<10; i++){
        add_surfaces(surfaces, a1);
        add_surfaces(surfaces, a2);
        for(int j=0; j<4; j++){
            a1[j].setY(a1[j].getY()+30);
            a2[j].setY(a2[j].getY()+30);
        }
    }

    // 2: input the bus
    //  vertex of the bus
    Vector2D v_bus(0,20);
    vector<Point2D> bus = {Point2D(25,0, v_bus), Point2D(25, 10, v_bus),
                           Point2D(30, 10, v_bus), Point2D(30,0, v_bus)};
    add_surfaces(surfaces, bus);

    //3: input: transmitter and receiver
//    Vector2D v_transmitter(0, 0), v_receiver(0,0);
    Vector2D v_transmitter(0, 10), v_receiver(0,-10);
    Point2D transmitter(35,10, v_transmitter);
    Point2D receiver(45, 290, v_receiver);

    MultiPath model(surfaces, transmitter, receiver);

    //4: update the multipath by 0.1 second, and show all the paths in each time

    float t=0.1; //1 second
    for(int i=0; i<300; i++){
        cout << "time: " <<  i*t << " seconds." << endl;
//        // Show the LOS
        if(model.isLos()){
            cout<<"LOS: " << endl;
            model.getTransmitter().display();
            model.getReceiver().display();
        } else{
            cout << "LOS: None" << endl;
        }

        // Show the order1 reflection point

        if(model.getReflectionOrder1Valid().empty()){
            cout << "path_reflect_order1 : None" << endl;
        } else{
            cout << "path_reflect_order1 :" << endl;
            int n = 0;
            for(auto path : model.getReflectionOrder1Valid()){
                cout << "path" << n << ": " <<endl;
                n++;
                path.reflectionPoint.point.display();
            }
        }

        if(model.getReflectionOrder2Valid().empty()){
            cout << "path_reflect_order2 : None" << endl;
        } else{
            cout << "path_reflect_order2 :" << endl;
            int n=0;
            for(auto  paths: model.getReflectionOrder2Valid()){
                cout << "paths" << n << ": " <<endl;
                n++;
                cout<< "Reflection Point 1: " << endl;
                paths[0].reflectionPoint.point.display();
                cout<< "Reflection Point 2: " << endl;
                paths[1].reflectionPoint.point.display();
            }
        }

        cout<<"life: " << model.getLife()<<endl;
        cout<< "" <<endl;

        model.update(t);
    }
}

// compare the final results between DRT and traditional RT
void test7(){
    // the test is in a road 30x300 meter, with buildings beside of the road
    // the buildings are about 20x10, and 10 buildings on each side.
    // 1: input: buildings surfaces
    vector<Surface2D> surfaces ={};
    // four points representing the bottom buildings on the left and right sides
    vector<Point2D> a1 = {Point2D(10,10), Point2D(10, 30), Point2D(20, 30), Point2D(20,10)};
    vector<Point2D> a2 = {Point2D(50,10), Point2D(50, 30), Point2D(60, 30), Point2D(60,10)};
    // There are 20 buildings in this test, and a loop is used to generate each building and add its surface to the surface array.
    // Each building is the same size and the distance between each building is 10 meters.
    for(int i=0; i<10; i++){
        add_surfaces(surfaces, a1);
        add_surfaces(surfaces, a2);
        for(int j=0; j<4; j++){
            a1[j].setY(a1[j].getY()+30);
            a2[j].setY(a2[j].getY()+30);
        }
    }

    // 2: input the bus
    //  vertex of the bus
    Vector2D v_bus(0,20);
    vector<Point2D> bus = {Point2D(25,0, v_bus), Point2D(25, 10, v_bus),
                           Point2D(30, 10, v_bus), Point2D(30,0, v_bus)};
    add_surfaces(surfaces, bus);

    //3: input: transmitter and receiver
//    Vector2D v_transmitter(0, 0), v_receiver(0,0);
    Vector2D v_transmitter(0, 10), v_receiver(0,-10);
    Point2D transmitter(35,10, v_transmitter);
    Point2D receiver(45, 290, v_receiver);

    MultiPath model_DRT(surfaces, transmitter, receiver);
    MultiPath model_RT(surfaces, transmitter, receiver);

    //4: update the multipath by 0.1 second, and show all the paths in each time
    float t=0.1; //1 second
    for(int i=0; i<300; i++){
        cout << "time: " <<  i*t << " seconds." << endl;

        //compare the reflection order1
        vector<Path> order1_RT = model_RT.reflection_order1_valid_RT();
        vector<Path> order1_DRT = model_DRT.getReflectionOrder1Valid();

        if(order1_RT.size() != order1_DRT.size()){
            cout << "the number of path in order1 is different!" << endl;
        } else{
            for(int j=0; j<order1_RT.size(); j++){
                if((order1_RT[j].reflectionPoint.point.getX() != order1_DRT[j].reflectionPoint.point.getX())
                or (order1_RT[j].reflectionPoint.point.getY() != order1_DRT[j].reflectionPoint.point.getY())){
                    cout << "the " << j << "paths of order1 are different"<< endl;
                    order1_RT[j].reflectionPoint.point.display();
                    order1_DRT[j].reflectionPoint.point.display();
                }
            }
        }

        // compare the reflection order2
        vector<vector<Path>> order2_RT = model_RT.reflection_order2_valid_RT();
        vector<vector<Path>> order2_DRT = model_DRT.getReflectionOrder2Valid();

        if(order2_RT.size() != order2_DRT.size()){
            cout << "the number of path in order1 is different!" << endl;
        } else{
            for(int j=0; j<order2_RT.size(); j++){
                if((order2_RT[j][0].reflectionPoint.point.getX() != order2_DRT[j][0].reflectionPoint.point.getX())
                    or (order2_RT[j][0].reflectionPoint.point.getX() != order2_DRT[j][0].reflectionPoint.point.getX())
                    or (order2_RT[j][1].reflectionPoint.point.getX() != order2_DRT[j][0].reflectionPoint.point.getX())
                    or (order2_RT[j][1].reflectionPoint.point.getX() != order2_DRT[j][0].reflectionPoint.point.getX())){
                    cout << "the " << j << "paths of order2 are different"<< endl;
                    order2_RT[j][0].reflectionPoint.point.display();
                    order2_DRT[j][0].reflectionPoint.point.display();
                    order2_RT[j][1].reflectionPoint.point.display();
                    order2_DRT[j][1].reflectionPoint.point.display();
                }
            }
        }

        cout << ""<<endl;

        model_RT.update(t);
        model_DRT.update(t);
    }
}

// compare the number of multi path between DRT and traditional RT
void test8(){
    // the test is in a road 30x300 meter, with buildings beside of the road
    // the buildings are about 20x10, and 10 buildings on each side.
    // 1: input: buildings surfaces
    vector<Surface2D> surfaces ={};
    // four points representing the bottom buildings on the left and right sides
    vector<Point2D> a1 = {Point2D(10,10), Point2D(10, 30), Point2D(20, 30), Point2D(20,10)};
    vector<Point2D> a2 = {Point2D(50,10), Point2D(50, 30), Point2D(60, 30), Point2D(60,10)};
    // There are 20 buildings in this test, and a loop is used to generate each building and add its surface to the surface array.
    // Each building is the same size and the distance between each building is 10 meters.
    for(int i=0; i<10; i++){
        add_surfaces(surfaces, a1);
        add_surfaces(surfaces, a2);
        for(int j=0; j<4; j++){
            a1[j].setY(a1[j].getY()+30);
            a2[j].setY(a2[j].getY()+30);
        }
    }

    // 2: input the bus
    //  vertex of the bus
    Vector2D v_bus(0,20);
    vector<Point2D> bus = {Point2D(25,0, v_bus), Point2D(25, 10, v_bus),
                           Point2D(30, 10, v_bus), Point2D(30,0, v_bus)};
    add_surfaces(surfaces, bus);

    //3: input: transmitter and receiver
//    Vector2D v_transmitter(0, 0), v_receiver(0,0);
    Vector2D v_transmitter(0, 10), v_receiver(0,-10);
    Point2D transmitter(35,10, v_transmitter);
    Point2D receiver(45, 290, v_receiver);

    MultiPath model_DRT(surfaces, transmitter, receiver);
    MultiPath model_RT(surfaces, transmitter, receiver);

    vector<unsigned long> number_DRT={}, number_RT={};

    //4: update the multipath by 0.1 second, and show all the paths in each time
    float t=0.1; //1 second
    for(int i=0; i<300; i++){
        cout << "time: " <<  i*t << " seconds." << endl;

        //compare the reflection order1
        vector<Path> order1_RT = model_RT.reflection_order1_valid_RT();
        vector<Path> order1_DRT = model_DRT.getReflectionOrder1Valid();

        // compare the reflection order2
        vector<vector<Path>> order2_RT = model_RT.reflection_order2_valid_RT();
        vector<vector<Path>> order2_DRT = model_DRT.getReflectionOrder2Valid();

        number_DRT.push_back((order1_DRT.size() + order2_DRT.size()));
        number_RT.push_back((order1_RT.size()+ order2_RT.size()));

        model_RT.update(t);
        model_DRT.update(t);
    }
    cout<<"n_DRT = [";
    for(auto value: number_DRT){
        cout << value << ", ";
    }
    cout<< endl;

    cout<<"n_RT = [";
    for(auto value: number_RT){
        cout << value << ", ";
    }
    cout<< endl;

}


int main() {
    test8();
    return 0;
}
