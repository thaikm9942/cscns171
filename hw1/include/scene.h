#ifndef __SCENE_H__
#define __SCENE_H__

#include "./object.h"
#include "./transform.h"

/* 
 * This header file defines the Camera, Perspective and Scene classes, which
 * will be used to represent the scene and space we are drawing Objects in
 */

class Perspective {
    // Perspective parameters
    double n_, f_, l_, r_, t_, b_;

    public:
        // Default constructor
        Perspective() : n_(0.0), f_(0.0), l_(0.0), r_(0.0), t_(0.0), b_(0.0) {}

        // Constructor that takes in given parameters
        Perspective(double n, double f, double l, double r, double t, double b) 
            : n_(n), f_(f), l_(l), r_(r), t_(t), b_(b) {}

        // Compute the perspective matrix from the given parameters
        Matrix4d compute_perspective_matrix();

        // Print the text representing a Perspective object
        void print_perspective();
};

class Camera {
    // Camera position in world space
    double p_[3];

    // Camera orientation in world space
    double o_[3];
    double angle_;

    public:
        // Default constructor
        Camera() : angle_(0.0) {
            p_[0] = 0.0, p_[1] = 0.0, p_[2] = 0.0;
            o_[0] = 0.0, o_[1] = 0.0, o_[2] = 0.0;           
        }

        // Constructor that takes in given parameters
        Camera(double p[3], double o[3], double angle) : angle_(angle) {
            p_[0] = p[0], p_[1] = p[1], p_[2] = p[2];
            o_[0] = o[0], o_[1] = o[1], o_[2] = o[2];  
        }

        // Computes the transformation matrix representing the camera
        Transformation compute_camera_matrix();

        // Prints the text representing a Camera object
        void print_camera();
};

class Scene {
    Camera cam_;
    Perspective perp_;
    vector<Labeled_Object> objs_;

    public:
        // Default constructor
        Scene() : cam_(), perp_(), objs_() {}

        // Constructor given a Camera and Perspective object
        Scene(Camera cam, Perspective perp) : cam_(cam), perp_(perp), objs_() {}

        // Add a labeled object to the scene given an object and its label
        void add_labeled_object(Object obj, string label);

        // Add an existing Labeled_Object item to the list of labeled objects
        void add_labeled_object(Labeled_Object l_obj);

        // Prints the text representing a Scene object
        void print_scene();
};

#endif // #ifndef __SCENE_H__