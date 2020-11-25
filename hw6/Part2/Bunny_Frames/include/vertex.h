#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <math.h>

/* 
 * This header file defines the Vertex class and a couple of operations
 * between Vertex objects
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


// Default NULL_VERTEX to mark the start of a vector of Vertex objects since vertices are 1-indexed
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

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

#endif // #ifndef __VERTEX_H__
