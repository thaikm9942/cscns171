#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>
#include <math.h>
#include "./vertex.h"

using namespace std;

/* 
 * This header file defines the Face and Object classes, which
 * will be used to represent .obj files, and includes a few functions
 * on Object objects.
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

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

/* This class represents a graphical object which contains the vertices,
 * the vertex normals, the faces of an object, and its corresponding normal
 * and vertex buffers
 */
class Object {
    public:
        // These are generated using the parser
        vector<Vertex> vertices;
        vector<Face> faces;

        // These are generated using the halfedge data structure
        vector<Vertex> vertex_buffer;
        vector<Vertex> normal_buffer;
        
        // Default constructor that initializes empty lists of vertices and
        // faces and empty vertex and normal buffers.
        Object() : vertices(), faces(), vertex_buffer(), normal_buffer() {}

        // Constructor that takes in only a list of vertices and faces that
        // forms the object.
        Object(vector<Vertex> vs, vector<Face> fs) : vertices(vs), faces(fs), vertex_buffer(), normal_buffer() {}

        // Copy constructor for an Object class
        Object(const Object& other) : vertices(other.vertices), faces(other.faces), 
            vertex_buffer(other.vertex_buffer), normal_buffer(other.normal_buffer) {}

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

#endif // #ifndef __OBJECT_H__
