#include "../include/object.h"

void Object::add(Vertex v) { 
    vertices.push_back(v); 
}

void Object::add(Face f) { 
    faces.push_back(f); 
}