#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>
#include <set>
#include <tuple>
#include "./transform.h"

using namespace std;

/* 
 * This header file defines the Vertex, Face and Object classes, which
 * will be used to represent .obj files, and includes a few functions
 * on Object objects. It also contains the Material class,
 * which is used to represent the different reflectance properties and 
 * the shininess of an object
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

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

        // Converts from homogeneous coordinates to Cartesian coordinates by dividing
        // all coordinates by w
        void to_cartesian();

        // Normalize a vertex
        void normalize();

        // Checks if a NDC coordinate is contained within the perspective cube
        bool is_contained();

        // Converts a vertex in NDC coordinates to a vertex in screen space
        Vertex to_screen_coordinates(int xres, int yres);
};

// Default NULL_VERTEX to mark the start of a vector of Vertex objects since vertices are 1-indexed
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

/* This class represents a face containing 2 sets of 3 integers, 1 representing 
 * the 3 1-indexed vertices forming that face and the other representing
 * the 3 1-indexed surface normals of each vertex that forms the face.
 */ 
class Face {
    public:
        // Indices of the vertices that form the face
        int i_[3];

        // Indices of the vertex normals for each vertex
        int n_[3];
        
        // Constructor that only takes in 3 integers corresponding to
        // the 3 indices of vertices that form the face. No normal
        // indices are passed in
        Face(int i[3]) : n_() {
            i_[0] = i[0], i_[1] = i[1], i_[2] = i[2];
        }

        // Constructor that takes in 3 integers corresponding to
        // the 3 indices of vertices that form the face and their
        // corresponding vertex normal indices
        Face(int i[3], int n[3]) : Face(i) {
            n_[0] = n[0], n_[1] = n[1], n_[2] = n[2];
        }
        
        // Print text representing the Face object
        void print_face();
};

// This class defines different material properties of an Object, including ambient material reflectance,
// diffuse material reflectance, specular material reflectance, and material "shininess"
class Material {
    public:
        // Ambient material reflectance
        double a_[3];
        // Diffuse material reflectance
        double d_[3];
        // Specular material reflectance
        double s_[3];
        // Shininess of the object (or Phong exponent)
        double p_;
        
        // Default constructor
        Material() : p_(0.0) { 
            a_[0] = 0.0, a_[1] = 0.0, a_[2] = 0.0;
            d_[0] = 0.0, d_[1] = 0.0, d_[2] = 0.0;
            s_[0] = 0.0, s_[1] = 0.0, s_[2] = 0.0;
        }

        // Constructor that takes in given parameters.
        Material(double a[3], double d[3], double s[3], double p) : p_(p) {
            a_[0] = a[0], a_[1] = a[1], a_[2] = a[2];
            d_[0] = d[0], d_[1] = d[1], d_[2] = d[2];
            s_[0] = s[0], s_[1] = s[1], s_[2] = s[2];
        }

        // Outputs the Material object as a string representation
        void print_material();
};

/* This class represents a graphics object which contains the verticesm
 * the vertex normals, the faces of an object, and the transformations 
 * to be applied to all vertices
 */
class Object {
    public:
        vector<Vertex> vs_;
        vector<Vertex> vns_;
        vector<Face> fs_;
        Transformation t_;
        Material m_;

        // Default constructor that initializes empty lists of vertices,
        // faces and transforms for the object
        Object() : vs_(), vns_(), fs_(), t_(), m_() {}

        // Constructor that takes in only a list of vertices and faces that
        // forms the object. The list of transformation matrices are initialized,
        // but left empty.
        Object(vector<Vertex> vs, vector<Vertex> vns, vector<Face> fs) : vs_(vs), vns_(vns), fs_(fs), t_(), m_() {}

        // Copy constructor for an Object class
        Object(const Object& other) : vs_(other.vs_), vns_(other.vns_), fs_(other.fs_), t_(other.t_), m_(other.m_)  {}

        // Functions to add a vertex normal, vertex, a face or a transformation matrix to the corresponding
        // list of the object
        void add_normal(Vertex vn);
        void add(Vertex v);
        void add(Face f);

        // Return a vertex at the given index i
        Vertex get_vertex(int i);

        // Return a vertex normal at the given index in
        Vertex get_vertex_normal(int in);

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

// Multiplying a vector by a scalar returns a scaled vector
Vertex dot(double scalar, Vertex v);

// Dot product of 2 vectors (represented as Vertex)
double dot(Vertex v1, Vertex v2);

// Cross product of 2 vectors (represented as Vertex)
Vertex cross(Vertex v1, Vertex v2);

// Add 2 vertices together to obtain a new vertex
Vertex add(Vertex v1, Vertex v2);

// Subtracts one vertex from another vertex to obtain a new vertex
Vertex subtract(Vertex v1, Vertex v2);

// Compute the distance squared between 2 vertices
double compute_distance_squared(Vertex v1, Vertex v2);

// This function returns the final vertex normals of the Object after applying
// the corresponding Transformation to the current Object's vertex normals
vector<Vertex> get_transformed_normals(Object obj);

// This function returns the final vertices of the object after applying
// the corresponding Transformation to the current Object's vertices
vector<Vertex> get_transformed_vertices(Object obj);

// This function applies a specific transformation matrix to the
// vertices of an Object and returns the final vertices
vector<Vertex> get_transformed_vertices(Object obj, Matrix4d transform);

// Convert a Vertex to a 4 x 1 column vector
Vector4d to_col_vector(Vertex v);

// Convert a 4 x 1 column vector to a Vertex
Vertex to_vertex(Vector4d col);

#endif // #ifndef __OBJECT_H__
