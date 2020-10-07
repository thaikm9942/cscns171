#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>
#include "../Eigen/Dense"

using namespace Eigen;
using namespace std;

// This class represents a vertex with x, y, and z float coordinates 
class Vertex {
    public:
        float x_, y_, z_;
        // Default constructor. Initializes a vertex with
        // (0.0, 0.0, 0.0) coordinates
        Vertex() : x_(0.0), y_(0.0), z_(0.0) {}

        // Constructor that takes in a given (x, y, z) coordinates
        // to form a vertex
        Vertex(float x, float y, float z) : x_(x), y_(y), z_(z) {}
};

/* This class represents a face containing 3 integers representing 
 * the 3 1-indexed vertices forming that face.
 */ 
class Face {
    public:
        int i1_, i2_, i3_;
        
        // Constructor that takes in 3 integers corresponding to
        // the 3 indices of vertices that form the face
        Face(int i1, int i2, int i3) : i1_(i1), i2_(i2), i3_(i3) {}
};

/* This class represents a graphics object which contains the vertices,
 * the faces of an object, and the transformations to be applied to all
 * vertices
 */
class Object {
    public:
        vector<Vertex> vertices;
        vector<Face> faces;
        vector<Matrix4d> transforms;

        // Default constructor that initializes empty lists of vertices,
        // faces and transforms for the object
        Object() : vertices(), faces(), transforms() {}

        // Constructor that takes in only a list of vertices and faces that
        // forms the object. The list of transformation matrices are initialized,
        // but left empty.
        Object(vector<Vertex> vtx, vector<Face> fs) : vertices(vtx), faces(fs), transforms() {}

        // Functions to add a vertex, a face or a transformation matrix to the corresponding
        // list of the object
        void add(Vertex v) { vertices.push_back(v); }
        void add(Face f) { faces.push_back(f); }
        void add(Matrix4d m) {transforms.push_back(m); }
};

#endif // #ifndef __OBJECT_H__
