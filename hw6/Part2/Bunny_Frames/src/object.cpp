#include "../include/object.h"

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
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

void Object::add_vertex_to_buffer(Vertex v) {
    vertex_buffer.push_back(v);
}

void Object::add_normal_to_buffer(Vertex vn) {
    normal_buffer.push_back(vn);
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

    printf("vertex buffer:\n");
    for(size_t i = 0; i < vertex_buffer.size(); i++) {
        vertex_buffer[i].print_vertex();
    }

    printf("normal buffer:\n");
    for(size_t i = 0; i < normal_buffer.size(); i++) {
        normal_buffer[i].print_vertex();
    }
}