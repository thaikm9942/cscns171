#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>
#include <math.h>
#include "./transform.h"

using namespace std;

/* 
 * This header file defines the Vertex and Object classes, which
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
        // Coordinates of the vertex
        float x, y, z;

        // Default constructor. Initializes a vertex with
        // (0.0, 0.0, 0.0, 1.0) coordinates
        Vertex() : x(0.0), y(0.0), z(0.0) {}

        // Constructor that takes in a given (x, y, z) coordinates
        // to form a vertex. The default w coordinate is 1.0
        Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

        // Print text representing a Vertex object
        void print_vertex();
};

/* This class represents a face containing 2 sets of 3 integers, 1 representing 
 * the 3 1-indexed vertices forming that face and the other representing
 * the 3 1-indexed surface normals of each vertex that forms the face.
 */ 
class Face {
    public:
        // Indices of the vertices that form the face
        int idx1, idx2, idx3;

        // Constructor that only takes in 3 integers corresponding to
        // the 3 indices of vertices that form the face. No normal
        // indices are passed in
        Face(int i1, int i2, int i3) : idx1(i1), idx2(i2), idx3(i3) {}
        
        // Print text representing the Face object
        void print_face();
};

// Default NULL_VERTEX to mark the start of a vector of Vertex objects since vertices are 1-indexed
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

// This class defines different material properties of an Object, including ambient material reflectance,
// diffuse material reflectance, specular material reflectance, and material "shininess"
class Material {
    public:
        // Ambient material reflectance
        float ambient[3];

        // Diffuse material reflectance
        float diffuse[3];

        // Specular material reflectance
        float specular[3];

        // Shininess of the object (or Phong exponent)
        float shininess;
        
        // Default constructor
        Material() : shininess(0.0) { 
            ambient[0] = 0.0, ambient[1] = 0.0, ambient[2] = 0.0;
            diffuse[0] = 0.0, diffuse[1] = 0.0, diffuse[2] = 0.0;
            specular[0] = 0.0, specular[1] = 0.0, specular[2] = 0.0;
        }

        // Constructor that takes in given parameters.
        Material(float a[3], float d[3], float s[3], float p) : shininess(p) {
            ambient[0] = a[0], ambient[1] = a[1], ambient[2] = a[2];
            diffuse[0] = d[0], diffuse[1] = d[1], diffuse[2] = d[2];
            specular[0] = s[0], specular[1] = s[1], specular[2] = s[2];
        }

        // Outputs the Material object as a string representation
        void print_material();
};

/* This class represents a graphical object which contains the vertices,
 * the vertex normals, the faces of an object, and the transformation sets 
 * to be applied to all vertices
 */
class Object {
    public:
        // These are generated using the parser
        vector<Vertex> vertices;
        vector<Face> faces;

        // These are generated using the halfedge data structure
        vector<Vertex> vertex_buffer;
        vector<Vertex> normal_buffer;
        
        vector<Transform_Set> transforms;
        Material material;

        // Default constructor that initializes empty lists of vertices,
        // faces and transforms for the object
        Object() : vertices(), faces(), vertex_buffer(), normal_buffer(), transforms(), material() {}

        // Constructor that takes in only a list of vertices and faces that
        // forms the object. The list of transformation matrices are initialized,
        // but left empty.
        Object(vector<Vertex> vs, vector<Face> fs) : vertices(vs), faces(fs), vertex_buffer(), normal_buffer(), 
            transforms(), material() {}

        // Copy constructor for an Object class
        Object(const Object& other) : vertices(other.vertices), faces(other.faces), 
            vertex_buffer(other.vertex_buffer), normal_buffer(other.normal_buffer), 
            transforms(other.transforms), material(other.material) {}

        // Add a vertex normal to the vector of vertex normals
        void add_normal(Vertex vn);

        // Add a vertex to the vector of vertices
        void add(Vertex v);

        // Add a face to the vector of faces
        void add_face(Face f);

        // Add a vertex to the vertex buffer array
        void add_vertex_to_buffer(Vertex v);

        // Add a vertex to the normal buffer array
        void add_normal_to_buffer(Vertex v);

        // Return a vertex at the given index i
        Vertex get_vertex(int i);

        // Return a vertex normal at the given index in
        Vertex get_vertex_normal(int in);

        // Print text representing the Object object
        void print_object();
};


//////////////////////////////
///       STRUCTS          ///
//////////////////////////////

// Structs copied over from provided "struct.h"
struct Mesh_Data
{
    vector<Vertex*> *vertices;
    vector<Face*> *faces;
};

struct Vec3f
{
	float x, y, z;
};

// Labeled_Object struct
struct Labeled_Object {
    string label;
    Object obj;
    Mesh_Data* mesh_data;
};

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

// Calculate the sum of 2 vertices
Vertex add(Vertex v1, Vertex v2);

// Calculate the scalar product of a scalar and a vertex
Vertex dot(float c, Vertex v);

// Compute the Euclidean norm of 2 vectors
float norm(Vertex v);

// Calculate the dot product between 2 vectors
float dot(Vertex v1, Vertex v2);

// Calculate the cross product between 2 vectors 
Vertex cross(Vertex v1, Vertex v2);

// Normalize the vertex
void normalize(Vertex &v);

#endif // #ifndef __OBJECT_H__
