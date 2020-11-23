#include "../include/object.h"

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

void Face::print_face() {
    printf("i1: %d, i2: %d, i3: %d\n", idx1, idx2, idx3);
    printf("\n");
}

void Object::add(Vertex v) { 
    vertices.push_back(v); 
}

void Object::add_face(Face f) {
    faces.push_back(f);
}

/*
void Object::add_normal(Vertex vn) {
    vertex_normals.push_back(vn);
}
*/

void Object::add_vertex_to_buffer(Vertex v) {
    vertex_buffer.push_back(v);
}

void Object::add_normal_to_buffer(Vertex vn) {
    normal_buffer.push_back(vn);
}

Vertex Object::get_vertex(int i) {
    return vertex_buffer[i];
}

Vertex Object::get_vertex_normal(int in) {
    return normal_buffer[in];
}

void Object::print_object() {
    printf("vertices:\n");
    for(size_t i = 0; i < vertices.size(); i++) {
        vertices[i].print_vertex();
    }

    printf("faces:\n");
    for(size_t i = 0; i < faces.size(); i++) {
        faces[i].print_face();
    }

    /*
    printf("normals:\n");
    for(size_t i = 0; i < vertex_normals.size(); i++) {
        vertex_normals[i].print_vertex();
    }
    */

    printf("vertex buffer:\n");
    for(size_t i = 0; i < vertex_buffer.size(); i++) {
        vertex_buffer[i].print_vertex();
    }

    printf("normal buffer:\n");
    for(size_t i = 0; i < normal_buffer.size(); i++) {
        normal_buffer[i].print_vertex();
    }

    printf("transformations:\n");
    for(size_t i = 0; i < transforms.size(); i++) {
        printf("transform_set%zu:\n", i);
        transforms[i].print_transform_set();
    }
    
    printf("\n");
    printf("material:\n");
    material.print_material();
}