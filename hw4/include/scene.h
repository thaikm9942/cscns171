#ifndef __SCENE_H__
#define __SCENE_H__

#include "./light.h"

/* 
 * This header file defines the Camera, Perspective and Scene classes, which
 * will be used to represent the scene and space we are drawing Objects in
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class defines the paramters needed for a perspective projection matrix
class Perspective {
    public:
        // Perspective parameters
        float near, far, left, right, top, bottom;
        
        // Default constructor
        Perspective() : near(0.0), far(0.0), left(0.0), right(0.0), top(0.0), bottom(0.0) {}

        // Constructor that takes in given parameters
        Perspective(float n, float f, float l, float r, float t, float b) 
            : near(n), far(f), left(l), right(r), top(t), bottom(b) {}

        // Print the text representing a Perspective object
        void print_perspective();
};

// This class defines the orientation and position of the camera
class Camera {
    public:
        // Camera orientation in world space
        float orientation[3];

        // Camera angle
        float angle;

        // Camera position in world space
        float position[3];

        // Default constructor
        Camera() : angle(0.0) {
            position[0] = 0.0, position[1] = 0.0, position[2] = 0.0;
            orientation[0] = 0.0, orientation[1] = 0.0, orientation[2] = 0.0;           
        }

        // Constructor that takes in given parameters
        Camera(float p[3], float o[3], float angle) : angle(angle) {
            position[0] = p[0], position[1] = p[1], position[2] = p[2];
            orientation[0] = o[0], orientation[1] = o[1], orientation[2] = o[2];  
        }

        // Prints the text representing a Camera object
        void print_camera();
};

// This class defines the scene that depicts how we view the object.
// Each scene includes a camera position, the perspective view
// the list of objects to be drawn, and the light sources, and
// the screen resolutions.
class Scene {
    public:
        Camera camera;
        Perspective perspective;
        vector<Labeled_Object> objects;
        vector<Light> light_sources;
        int xres, yres;

        // Default constructor
        Scene() : camera(), perspective(), objects(), light_sources(), xres(0), yres(0) {}

        // Constructor given a Camera and Perspective object
        Scene(Camera cam, Perspective persp, vector<Light> ls, int xres, int yres) : camera(cam), perspective(persp), objects(), light_sources(ls), xres(xres), yres(yres) {}

        // Add a labeled object to the scene given an object and its label
        void add_labeled_object(Object obj, string label);

        // Add an existing Labeled_Object item to the list of labeled objects
        void add_labeled_object(Labeled_Object l_obj);

        // Prints the text representing a Scene object
        void print_scene();
};

#endif // #ifndef __SCENE_H__