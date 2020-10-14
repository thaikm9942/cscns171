#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>
#include <set>
#include <tuple>
#include "../include/transform.h"
#include "../include/pixel.h"

using namespace std;

/* 
 * This header file defines the Vertex, Face and Object classes, which
 * will be used to represent .obj files, and includes a few functions
 * on Object objects
 */

// This class represents a vertex with x, y, z and w homogeneous float coordinates 
class Vertex {
    public:
        double x_, y_, z_, w_;
        // Default constructor. Initializes a vertex with
        // (0.0, 0.0, 0.0, 1.0) coordinates
        Vertex() : x_(0.0), y_(0.0), z_(0.0), w_(1.0) {}

        // Constructor that takes in a given (x, y, z) coordinates
        // to form a vertex. The default w coordinate is 1.0
        Vertex(double x, double y, double z) : x_(x), y_(y), z_(z), w_(1.0) {}

        // Constructor that takes in a given homogenous (x, y, z, w) coordinate
        // to form a vertex.
        Vertex(double x, double y, double z, double w) : x_(x), y_(y), z_(z), w_(w) {}

        // Print text representing the Vertex object
        void print_vertex();

        // Converts from homogenous coordinates to Cartesian coordinates by dividing
        // all coordinates by w
        void to_cartesian();

        // Checks if a NDC coordinate is contained within the perspective cube
        bool is_contained();

        // Converts a vertex in world space to a vertex in screen space
        Vertex to_screen_coordinates(int xres, int yres);
};

// Default NULL_VERTEX to mark the start of a vector of Vertex objects since vertices are 1-indexed
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

/* This class represents a face containing 3 integers representing 
 * the 3 1-indexed vertices forming that face.
 */ 
class Face {
    public:
        int i1_, i2_, i3_;
        
        // Constructor that takes in 3 integers corresponding to
        // the 3 indices of vertices that form the face
        Face(int i1, int i2, int i3) : i1_(i1), i2_(i2), i3_(i3) {}
        
        // Print text representing the Face object
        void print_face();
};

/* This class represents a graphics object which contains the vertices,
 * the faces of an object, and the transformations to be applied to all
 * vertices
 */
class Object {
    public:
        vector<Vertex> vertices;
        vector<Face> faces;
        Transformation transform;
        // Default constructor that initializes empty lists of vertices,
        // faces and transforms for the object
        Object() : vertices(), faces(), transform() {}

        // Constructor that takes in only a list of vertices and faces that
        // forms the object. The list of transformation matrices are initialized,
        // but left empty.
        Object(vector<Vertex> vtx, vector<Face> fs) : vertices(vtx), faces(fs), transform() {}

        // Copy constructor for an Object class
        Object(const Object& other) : vertices(other.vertices), faces(other.faces), transform(other.transform) {}

        // Functions to add a vertex, a face or a transformation matrix to the corresponding
        // list of the object
        void add(Vertex v);
        void add(Face f);

        // Print text representing the Object object
        void print_object();

        // Return the screen coordinates of the corresponding object
        // by mapping all [-1, 1] x [-1, 1] coordinates to
        // [0, yres] x [0, xres]
        vector<Vertex> get_screen_coordinates(int xres, int yres);

        // Return a set of pixels to be drawn for the given object
        set<tuple<int, int>> get_pixels(int xres, int yres);
};

// TODO: Create Labeled_Object class with function find_object_with_label
// A struct containing an obj and its associated label

// Labeled_Object struct
struct Labeled_Object {
    string label;
    Object obj;
};

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

// This function returns the final vertices of the object after applying
// the corresponding Transformation to the current object's vertices
vector<Vertex> get_transformed_vertices(Object obj);

// This function applies a specific transformation matrix to the
// vertices of an object and returns the final vertices
vector<Vertex> get_transformed_vertices(Object obj, Matrix4d transform);

#endif // #ifndef __OBJECT_H__
