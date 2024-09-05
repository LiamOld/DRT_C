//
// Created by Lian Guo on 10/08/2024.
//

#ifndef DRT_2D_VERSION3_SURFACE2D_H
#define DRT_2D_VERSION3_SURFACE2D_H

#include "Vector2D.h"
#include "Point2D.h"
#include "Line2D.h"
#include "cmath"
#include "ReflectionPoint.h"

class Surface2D {
private:
    Point2D a, b;
    float A, B, C; // Ax+by=C
    Vector2D speed;
public:
    Surface2D(const Point2D &a, const Point2D &b, Vector2D speed= Vector2D(0,0) ) : a(a), b(b), speed(speed)
    {
        A = b.getY() - a.getY();
        B = a.getX() - b.getX();
        C = A * a.getX() + B * a.getY();
        speed = a.getSpeed();
    }

    void update(float t)
    {
        a.update(t);
        b.update(t);
        A = b.getY() - a.getY();
        B = a.getX() - b.getX();
        C = A * a.getX() + B * a.getY();
        speed = a.getSpeed();
    }

    Vector2D transformation(const Vector2D &v) const{
        // rotation
        float m = sqrt(A*A + B*B);
        float sin = A/m;
        float cos = -1 * (B/m);
        float x = v.getX()*cos + v.getY()*sin;
        float y = v.getY()*cos - v.getX()*sin;
        return {x,y};
    }
    // local coordinate: a -> (0,0)
    Point2D transformation(Point2D &p){
        Point2D origin(0,0);
        Vector2D translation_vector = origin - a;
        // translation
        Point2D p_t = p + translation_vector;
        // rotation
        float m = sqrt(A*A + B*B);
        float sin = A/m;
        float cos = -1 * (B/m);
        float x = p_t.getX()*cos + p_t.getY()*sin;
        float y = p_t.getY()*cos - p_t.getX()*sin;
        // speed transform
        Vector2D v = transformation(p.getSpeed() -this->speed );
        // acceleration transform
        Vector2D ac = transformation(p.getAcceleration());
        return {x,y,v, ac};
    }

    Vector2D reverse_transformation(const Vector2D &v) const{
        // rotation
        Point2D origin(0,0);
        float m = sqrt(A*A + B*B);
        float sin = -1*A/m;
        float cos = -1 * (B/m);
        float x = v.getX()*cos + v.getY()*sin;
        float y = v.getY()*cos - v.getX()*sin;
        return {x,y};
    }
    Point2D reverse_transformation(Point2D &p){
        // rotation
        Point2D origin(0,0);
        float m = sqrt(A*A + B*B);
        float sin = -1*A/m;
        float cos = -1 * (B/m);
        float x = p.getX()*cos + p.getY()*sin;
        float y = p.getY()*cos - p.getX()*sin;
        // speed transform
        Vector2D v = reverse_transformation(p.getSpeed()) + this->speed;
//        cout << "check if the v of local reverse tranform to global rightly?"<< endl;
//        cout << "v global: ";
//        v.display();
        // acceleration transform
        Vector2D ac = reverse_transformation(p.getAcceleration());
        Point2D p_new(x,y,v,ac );
//        cout << "check if the v of the new point right?";
//        p_new.getSpeed().display();
        // translation
        Vector2D reverse_translation_vector = a - origin;
        return p_new + reverse_translation_vector;
    }

    ReflectionPoint reflection_point(Point2D &t, Point2D &r){
        //1, translate the receiver and transmitter into the local coordinate
        Point2D t_local = transformation(t);
        Point2D r_local = transformation(r);

        //2, check if the two point in the same side of the surface
        if((t_local.getY()*r_local.getY())<0 or (t_local.getY()*r_local.getY())==0){
            Point2D empty(0,0);
            return {empty,0};
        }

        //3, get the coordinate of reflection point in the local coordinate
        float x_reflection_local =
                (t_local.getX()*r_local.getY() + r_local.getX()*t_local.getY())/
                (r_local.getY()+t_local.getY());
        Point2D reflection_local(x_reflection_local, 0);

        //3.1, check if the reflection point is inside the surface
        // if not, return a (0,0)point with max lifetime
        if(x_reflection_local < 0 or x_reflection_local > (a-b).magnitude()){
            return {Point2D(0,0), 10000000000.00};
        }

        //4, get the velocity of the reflection point in local coordinate
        float velocity_reflection_local_x = velocity_reflection_local(t_local, r_local);
//        cout << "check if the vx get right. v local x: " << velocity_reflection_local_x << endl;
        Vector2D velocity_reflection_local(velocity_reflection_local_x, 0);
//        cout << "check if the v Vector2D set right. v local vector: ";
//        velocity_reflection_local.display();
        reflection_local.setSpeed(velocity_reflection_local);
//        cout << "check if the speed station of the reflection_point set right. reflection point: ";
//        reflection_local.getSpeed().display();

        //5, get the acceleration of the reflection point in local coordinate
        //Assuming that the surface's acceleration is 0, so the a in local coordinate equal the one in global.
        float acceleration_reflection_x = acceleration_reflection_local(t_local, r_local);
        Vector2D acceleration_reflection_local(acceleration_reflection_x, 0);
        reflection_local.setAcceleration(acceleration_reflection_local);

        //6, transform the coordinate and the velocity from local coordinate into global coordinate
        Point2D reflection_global = reverse_transformation(reflection_local);
//        cout << "check if the speed station of the reflection_point set right. reflection point global: ";
//        reflection_global.getSpeed().display();

        //7, get the life of the reflection point
        // the x coordinate of point b in local coordinate is the length of the surface
        float b_local = (a-b).magnitude();
        float life = get_life(velocity_reflection_local_x, acceleration_reflection_x, x_reflection_local, b_local);

        //8, new a ReflectionPoint here, with the reflection_global and life.
        ReflectionPoint reflectionPoint_global(reflection_global, life);

        return reflectionPoint_global;
    }

    // this is calculated in the local coordinate
    // point a is at (0,0) and b is at (length, 0) and reflection point is at (x,0)
    // velocity and acceleration is just the component of x-axis
    static float get_life(float velocity, float acceleration, float x, float length){
        if(velocity * acceleration == 0){
            if(velocity == 0){
                if(acceleration == 0){
                    float max = 10000000000.00;
                    return max;
                }
                else if(acceleration > 0){
                    return sqrt((length-x)/acceleration);
                } else {
                    return sqrt(-1 * x/acceleration);
                }
            }
            else if(velocity > 0){
                return (length-x)/velocity;
            } else{
                return -1 * x/velocity;
            }
        }

            //in this case, both velocity and acceleration > 0 or <0
        else if(velocity * acceleration > 0){
            if(velocity > 0){
                return sqrt(2*(length-x)/acceleration + (velocity/acceleration)*(velocity/acceleration)) - (velocity/acceleration);
            } else {
                return sqrt(2*(0-x)/acceleration + (velocity/acceleration)*(velocity/acceleration)) - (velocity/acceleration);
            }
        }

            // in this case, velocity>0 and acceleration<0, or velocity<0 and acceleration>0
        else {
            // velocity>0 and acceleration<0
            if(velocity>0){
                //supposing the reflection point move towards the point b, and s is positive.
                float s = (length-x);
                float r = 2*s/acceleration + (velocity/acceleration)*(velocity/acceleration);
                // if r>0, it means that the reflection point could reach the point b.
                if(r>0){
                    // there will be two t, and the small one should be the right answer.
                    return -1*sqrt(r)-velocity/acceleration;
                }else{
                    // if r <= 0,it means that the reflection point could reach the point b
                    // and then calculate the time to reach the point a and s is nagetive.
                    s = (0-x);
                    r = 2*s/acceleration + (velocity/acceleration)*(velocity/acceleration);
                    return sqrt(r) - velocity/acceleration;
                }
            } else{
                // velocity<0 and acceleration>0
                // supposing the reflection point move towards the point a, and s is negative.
                float s = (0-x);
                float r = 2*s/acceleration + (velocity/acceleration)*(velocity/acceleration);
                if(r>0){
                    return -1*sqrt(r)-velocity/acceleration;
                } else{
                    s = (length-x);
                    r = 2*s/acceleration + (velocity/acceleration)*(velocity/acceleration);
//                    cout<<"r= "<< r;
//                    cout<<"v/a= " << velocity/acceleration;
                    return sqrt(r) - velocity/acceleration;
                }
            }
        }
    }

    // the velocity of the reflection point in local coordinate
    // return only component on the x-axis, as the one on the y-axis is 0
    static float velocity_reflection_local(Point2D t, Point2D r){
        //Derive the equation, x coordinate of reflection point, in above method.
        //the velocity composed by 4 parts, as following:
        float v_tx = r.getY() /(t.getY() + r.getY());
        float v_ty = r.getY() * ( r.getX() - t.getX() ) /
                     ((r.getY()+t.getY()) * (r.getY() + t.getY()));
        float v_rx = t.getY() / (t.getY() + r.getY());
        float v_ry = t.getY() * ( t.getX() - r.getX() ) /
                     ((r.getY()+t.getY()) * (r.getY() + t.getY()));

        return v_tx * t.getSpeed().x + v_ty * t.getSpeed().y + v_rx * r.getSpeed().x + v_ry * r.getSpeed().y;
    }

    static float acceleration_reflection_local (Point2D t, Point2D r){
        float sq_2 =  (r.getY()+t.getY()) * (r.getY() + t.getY());
        float sq_3 =  sq_2 * (r.getY() + t.getY());

        float a_tx_ty = -1 * r.getY() / sq_2; // tx ty
        float a_tx_ry = t.getY() / sq_2; // tx ry

        float a_rx_ty = -1 * a_tx_ty; // rx ty
        float a_rx_ry = -1 * a_tx_ry; // rx ry

        float a_ty_tx = a_tx_ty; // ty tx
        float a_ty_ty = 2*(t.getX()-r.getX())*r.getY()/sq_3; // ty ty
        float a_ty_rx = a_rx_ty; // ty rx
        float a_ty_ry = (r.getX()-t.getX())*(t.getY()-r.getY())/sq_3; // ty ry

        float a_ry_tx = a_tx_ry; // ry tx
        float a_ry_ty = a_ty_ry; // ry ty
        float a_ry_rx = a_rx_ry; // ry rx
        float a_ry_ry = 2*(r.getX()-t.getX())*t.getY()/sq_3; // ry ry

        float a_final =
                a_tx_ty * t.getSpeed().x * t.getSpeed().y +
                a_tx_ry * t.getSpeed().x * r.getSpeed().y +
                a_rx_ty * r.getSpeed().x * t.getSpeed().y +
                a_rx_ry * r.getSpeed().x * r.getSpeed().y +
                a_ty_tx * t.getSpeed().y * t.getSpeed().x +
                a_ty_ty * t.getSpeed().y * t.getSpeed().y +
                a_ty_rx * t.getSpeed().y * r.getSpeed().x +
                a_ty_ry * t.getSpeed().y * r.getSpeed().y +
                a_ry_tx * r.getSpeed().y * t.getSpeed().x +
                a_ry_ty * r.getSpeed().y * t.getSpeed().y +
                a_ry_rx * r.getSpeed().y * r.getSpeed().x +
                a_ry_ry * r.getSpeed().y * r.getSpeed().y;
        return a_final;
    }

    Point2D get_image(Point2D p){
        // translate p into local coordinate
        Point2D image = transformation(p);
        //reverse the coordinate and the velocity of y component
        //suppose the acceleration of p is 0
        image.setY(-1*image.getY());
        image.setSpeed( Vector2D(image.getSpeed().x, -1*image.getSpeed().y));
        image = reverse_transformation(image);
        return image;
    }

    // intersection check methods
    // get the intersection_point by traditional DR
    Point2D intersection_point(Line2D l) const
    {
        float det = A * l.getB() - l.getA() * B;
        return det == 0 ? Point2D(0, 0) : Point2D((l.getB() * C - B * l.getC()) / det, (A * l.getC() - l.getA() * C) / det);
    }

    // check if the intersection_point in the surface(2D)
    bool inside_check(Point2D p){
        return min(a.getY(), b.getY()) <= p.getY() && p.getY() <= max(a.getY(), b.getY()) &&
               min(a.getX(), b.getX()) <= p.getX() && p.getX() <= max(a.getX(), b.getX());
    }

    // check if a path is intersection with the surface, adding the checking if the intersection point is inside the path.
    bool intersection_check(Line2D l)
    {
        Point2D p = intersection_point(l);
        return inside_check(p) &&
               min(l.geta().getY(), l.getb().getY()) <= p.getY() && p.getY() <= max(l.geta().getY(), l.getb().getY()) &&
               min(l.geta().getX(), l.getb().getX()) <= p.getX() && p.getX() <= max(l.geta().getX(), l.getb().getX());
    }

    void display()
    {
        cout << "a.x: " << a.getX() << "; a.y: " << a.getY() << endl;
        cout << "b.x: " << b.getX() << "; b.y: " << b.getY() << endl;
    }

    const Point2D &getA() const {
        return a;
    }

    const Point2D &getB() const {
        return b;
    }

    const Vector2D &getSpeed() const {
        return speed;
    }
};


#endif //DRT_2D_VERSION3_SURFACE2D_H
