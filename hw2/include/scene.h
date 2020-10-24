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

// This class defines the orientation and position of the camera
class Camera {
    // Camera orientation in world space
    double o_[3];
    double angle_;

    public:
        // Camera position in world space
        double p_[3];

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
        Transformation compute_camera_transform();

        // Prints the text representing a Camera object
        void print_camera();
};

// This class defines the scene that depicts how we view the object.
// Each scene includes a camera position, the perspective view
// the list of objects to be drawn, and the light sources, and
// the screen resolutions.
class Scene {
    public:
        Camera cam_;
        Perspective persp_;
        vector<Labeled_Object> objs_;
        vector<Light> ls_;
        int xres_, yres_;

        // Default constructor
        Scene() : cam_(), persp_(), objs_(), ls_(), xres_(0), yres_(0) {}

        // Constructor given a Camera and Perspective object
        Scene(Camera cam, Perspective persp, vector<Light> ls, int xres, int yres) : cam_(cam), persp_(persp), objs_(), ls_(ls), xres_(xres), yres_(yres) {}

        // Set the light sources of the Scene object
        // Add a labeled object to the scene given an object and its label
        void add_labeled_object(Object obj, string label);

        // Add an existing Labeled_Object item to the list of labeled objects
        void add_labeled_object(Labeled_Object l_obj);

        // Prints the text representing a Scene object
        void print_scene();

        // Apply the geometric transformations to all vertices of the Objects.
        // The Objects still remain in world space coordinates.
        void apply_geometric_transforms();

        // Apply the camera transformations to all vertices of the Objects,
        // converting them from world space coordinates to camera space coordinates.
        void apply_camera_transforms();

        // Apply the perspective projection transform to all vertices of the Objects,
        // converting them from camera space coordiantes to NDC coordiantes
        void apply_perspective_transform();

        // Apply geometric transformations to the vertices and the vertex normals
        void apply_transformations();

        // Apply the geometric transformations (excluding translations) to all vertex normals
        // of the Objects.
        void transform_normals();

        // Only applicable when the vertices in the objects are NDC coordinates. Convert
        // the NDC coordinates of each object to screen coordinates
        void get_screen_coordinates();

        // Converts a vertex from world space coordinates to NDC coordinates
        Vertex to_ndc_coordinates(Vertex v);

        // Return a set of pixels to be drawn using each Object's screen coordinates
        set<tuple<int, int>> get_pixels();
};

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

// Runs Gouraud shading on the scene to output the final rasterized, colored image
void scene_gouraud_shading(Scene &scene, vector<vector<Color>> &pixels, vector<vector<double>> &buffer);

// Runs Phong shading on the scene to output the final rasterized, colored image
void scene_phong_shading(Scene &scene, vector<vector<Color>> &pixels, vector<vector<double>> &buffer);

#endif // #ifndef __SCENE_H__