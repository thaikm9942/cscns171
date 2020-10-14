#include "../include/object.h"

//////////////////////////////
///     HELPER FUNCTIONS   ///
//////////////////////////////

// Convert a Vertex to a 4 x 1 column vector
Vector4d to_col_vector(Vertex v) {
    Vector4d c;
    c << v.x_, v.y_, v.z_, v.w_;
    return c;
}

// Convert a 4 x 1 column vector to a Vertex
Vertex to_vertex(Vector4d col) {
    return Vertex(col[0], col[1], col[2], col[3]);
}

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////

void Vertex::print_vertex() {
    printf("x: %f, y: %f, z: %f, w: %f\n", x_, y_, z_, w_);
    printf("\n");
}

void Face::print_face() {
    printf("v%d v%d v%d\n", i1_, i2_, i3_);
    printf("\n");
}

void Object::add(Vertex v) { 
    vertices.push_back(v); 
}

void Object::add(Face f) { 
    faces.push_back(f); 
}

void Object::print_object() {
    printf("vertices:\n");
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].print_vertex();
    }

    printf("faces:\n");
    for(int i = 0; i < faces.size(); i++) {
        faces[i].print_face();
    }

    printf("transformation:\n");
    transform.print_transformation();
}

//////////////////////////////
///    OTHER FUNCTIONS     ///
//////////////////////////////

vector<Vertex> get_transformed_vertices(Object obj) {
    vector<Vertex> transformed_vertices;
    transformed_vertices.push_back(NULL_VERTEX);

    // Applies transformation to all vertices of the object
    for (int i = 1; i < obj.vertices.size(); i++) {
        Vertex v = obj.vertices[i];
        Vector4d col = to_col_vector(v);
        Vector4d final = obj.transform.compute_product() * col;
        Vertex final_v = to_vertex(final);
        transformed_vertices.push_back(final_v);
    }

    return transformed_vertices;
}

