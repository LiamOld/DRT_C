//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_MULTIPATH_H
#define DRT_2D_VERSION3_MULTIPATH_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Vector2D.h"
#include "Point2D.h"
#include "Line2D.h"
#include "Surface2D.h"
#include "Path.h"
#include "ReflectionPoint.h"
using namespace std;

class MultiPath {
private:
    vector<Surface2D> surfaces;
    Point2D transmitter, receiver;
    bool LOS;
    vector<Path> reflection_order1_valid;
    vector<vector<Path>> reflection_order2_valid;
    float life=10000000000.00;
    vector<Path> reflection_order1_oldPaths={};
    vector<vector<Path>> reflection_order2_oldPaths={};
    float life_oldPaths=0;
    unsigned long n;

public:
    MultiPath(const vector<Surface2D> &surfaces, const Point2D &transmitter, const Point2D &receiver) : surfaces(
            surfaces), transmitter(transmitter), receiver(receiver) {
        n = surfaces.size();
        LOS = block_check_LOS();
        reflection_order1_valid = reflection_order1_valid_RT();
        reflection_order2_valid = reflection_order2_valid_RT();
    }
    // t is the time difference
    // t is the time difference
    void update(float t){
        for(auto& f : surfaces){
            if(f.getSpeed().x!=0 or f.getSpeed().y!=0){
                f.update(t);
            }
        }
        transmitter.update(t);
        receiver.update(t);

        if (life <= 0 or block_check_order1() or block_check_order2()){
            // if the path was blocked, and the life>0, save the old paths
            // then do the RT to update the paths
            if(life > 0){
                life_oldPaths = life;
                reflection_order1_oldPaths=reflection_order1_valid;
                reflection_order2_oldPaths=reflection_order2_valid;

                cout << "the multipaths is blocked, and do the RT again!"<<endl;
                life = 1000000;
                LOS = block_check_LOS();
                reflection_order1_valid = reflection_order1_valid_RT();
                reflection_order2_valid = reflection_order2_valid_RT();
            }
                // if the life <0, check if life_oldPaths>0, if yes, means the old path is still alive
                // assign the old path to the current path, and check if it blocked.
                // if it is blocked, then do the RT again, update the paths.
                // if not blocked, means the old paths is still valid, update it.
            else{
                if(life_oldPaths>0){
                    reflection_order1_valid= reflection_order1_oldPaths;
                    reflection_order2_valid= reflection_order2_oldPaths;
                    if(block_check_order1() or block_check_order2()){
                        cout << "the multipaths is dead, and do the RT again!"<<endl;
                        life = 1000000;
                        LOS = block_check_LOS();
                        reflection_order1_valid = reflection_order1_valid_RT();
                        reflection_order2_valid = reflection_order2_valid_RT();
                    } else{
                        cout<<"the multipaths is dead, but the old path is still valid, use the old paths!"<<endl;
                        paths_update(t);
                    }
                } else {
                    cout << "the multipaths is dead, and do the RT again!"<<endl;
                    life = 1000000;
                    LOS = block_check_LOS();
                    reflection_order1_valid = reflection_order1_valid_RT();
                    reflection_order2_valid = reflection_order2_valid_RT();
                }
            }
        } else{
            paths_update(t);

            if(life_oldPaths > 0){
                old_paths_update(t);
            }
        }
    }


    void paths_update(float t){
        life -= t;

        LOS = block_check_LOS();

        for(auto& p: reflection_order1_valid){
            p.update(t);
        }
        for(auto& p: reflection_order2_valid){
            p[0].update(t);
            p[1].update(t);
        }
    }

    void old_paths_update(float t){
        life_oldPaths -= t;

        for(auto& p: reflection_order1_oldPaths){
            p.update(t);
        }
        for(auto& p: reflection_order2_oldPaths){
            p[0].update(t);
            p[1].update(t);
        }
    }

    bool if_blocked(Line2D l, int surface_index1 = -1, int surface_index2=-1){
        for(int k=0; k<n; k++){
            if(k==surface_index1 or k==surface_index2){
                continue;
            }
            if(surfaces[k].intersection_check(l)){
                return true;
            }
        }
        return false;
    }

    // true means los is not blocked
    bool block_check_LOS(){
        Line2D l(transmitter, receiver);
        return !(if_blocked(l));
    }

    // true means there some path is blocked.
    bool block_check_order1(){
        for(Path path: reflection_order1_valid){
            Line2D l1(transmitter, path.reflectionPoint.point);
            Line2D l2(path.reflectionPoint.point, receiver);
            if (if_blocked(l1, path.surface_index) or if_blocked(l2, path.surface_index)){
                return true;
            }
        }
        return false;
    }

    // true means there some path is blocked.
    bool block_check_order2(){
        for(vector<Path> paths: reflection_order2_valid){
            Line2D l1(transmitter, paths[0].reflectionPoint.point);
            Line2D l2(paths[0].reflectionPoint.point, paths[1].reflectionPoint.point);
            Line2D l3(paths[1].reflectionPoint.point, receiver);

            if(if_blocked(l1, paths[0].surface_index)
               or if_blocked(l2, paths[0].surface_index, paths[1].surface_index)
               or if_blocked(l3, paths[1].surface_index)){
                return true;
            }
        }
        return false;
    }

    vector<Path> reflection_order1_valid_RT(){
        //get every possible reflection point
        vector<Path> output = {};

        for(int i=0; i<n; i++ ){
            // get the reflection Point, with the point and its life
            ReflectionPoint p = surfaces[i].reflection_point(transmitter, receiver);
            // if (x,y)=(0,0), it means that the r and t are at the different side of the surface
            if(!(p.point.getX()==0 and p.point.getY()==0)){
                Line2D l1(transmitter, p.point);
                Line2D l2(p.point, receiver);
                if (!(if_blocked(l1, i) or if_blocked(l2, i))){
                    Path path(p, i);
                    output.push_back(path);
                    life = min(life, p.life);
                }
            }
        }
        return output;
    }

    vector<vector<Path>> reflection_order2_valid_RT(){
        vector<vector<Path>> output = {};
        vector<Path> one_path ={};

        // for each two surfaces, calculate if there is any path
        //surface i is the first reflection surface, and face j is the second one.
        for(int i=0; i < n; i++){
            for(int j=0; j<n; j++){
                // initializing one_path, avoiding using old data
                one_path ={};
                if(i==j){
                    continue;
                }
                // get the image of the transmitter by surface i
                Point2D t_image = surfaces[i].get_image(transmitter);
                ReflectionPoint reflectionPoint2 = surfaces[j].reflection_point(t_image, receiver);
                //check if the reflection point existed
                if(reflectionPoint2.point.getX()==0 and reflectionPoint2.point.getY()==0){
                    continue;
                }
                ReflectionPoint reflectionPoint1 = surfaces[i].reflection_point(transmitter, reflectionPoint2.point);
                //check if the reflection point existed
                if(reflectionPoint1.point.getX()==0 and reflectionPoint1.point.getY()==0){
                    continue;
                }
                // check if the path were blocked
                Line2D l1(transmitter, reflectionPoint1.point);
                Line2D l2(reflectionPoint1.point, reflectionPoint2.point);
                Line2D l3(reflectionPoint2.point, receiver);
                if(!(if_blocked(l1, i) or if_blocked(l2, i, j) or if_blocked(l3, j) )){
                    one_path.push_back(Path(reflectionPoint1, i));
                    one_path.push_back(Path(reflectionPoint2, j));
                    output.push_back(one_path);
                    life = min({reflectionPoint1.life, reflectionPoint2.life, life});
                }
            }
        }
        return output;
    }

    bool isLos() const {
        return LOS;
    }

    const vector<Path> &getReflectionOrder1Valid() const {
        return reflection_order1_valid;
    }

    const vector<vector<Path>> &getReflectionOrder2Valid() const {
        return reflection_order2_valid;
    }

    const Point2D &getTransmitter() const {
        return transmitter;
    }

    const Point2D &getReceiver() const {
        return receiver;
    }

    const vector<Surface2D> &getSurfaces() const {
        return surfaces;
    }

    float getLife() const {
        return life;
    }

    void setLife(float life) {
        MultiPath::life = life;
    }
};


#endif //DRT_2D_VERSION3_MULTIPATH_H
