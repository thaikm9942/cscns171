#include "../include/object.h"

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
void Vertex::print_vertex() {
    printf("x: %f, y: %f, z: %f\n", x, y, z);
    printf("\n");
}


void Object::add(Vertex v) { 
    vertices.push_back(v); 
}

void Object::add_normal(Vertex vn) {
    vertex_normals.push_back(vn);
}

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

    printf("normals:\n");
    for(size_t i = 0; i < vertex_normals.size(); i++) {
        vertex_normals[i].print_vertex();
    }

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