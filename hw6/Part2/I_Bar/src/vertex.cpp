#include "../include/vertex.h"
#include <stdio.h>

//////////////////////////////
///    USEFUL FUNCTIONS    ///
//////////////////////////////

// Calculate the sum of 2 vertices
Vertex add(Vertex v1, Vertex v2) {
    return Vertex(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// Calculate the scalar product of a scalar and a vertex
Vertex dot(float c, Vertex v) {
    return Vertex(c * v.x, c * v.y, c * v.z);
}

// Calculate the Euclidean norm of a vector
float norm(Vertex v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize the vertex
void normalize(Vertex &v) {
    float n = norm(v);
    v.x = v.x / n;
    v.y = v.y / n;
    v.z = v.z / n;
}

// Calculate the dot product between 2 vectors
float dot(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Calculate the cross product between 2 vectors 
Vertex cross(Vertex v1, Vertex v2) {
    double cx = v1.y * v2.z - v1.z * v2.y;
    double cy = v1.z * v2.x - v1.x * v2.z;
    double cz = v1.x * v2.y - v1.y * v2.x;
    return Vertex(cx, cy, cz);
}

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
void Vertex::print_vertex() {
    printf("x: %f, y: %f, z: %f\n", x, y, z);
    printf("\n");
}