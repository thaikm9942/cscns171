#include "../include/object.h"
#include "../include/bresenham.h"

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

void Vertex::to_cartesian() {
    x_ = x_ / w_;
    y_ = y_ / w_;
    z_ = z_ / w_;
    w_ = 1.0;
}

bool Vertex::is_contained() {
    // This assumes that the cube is axis-aligned, which is the case with the perspective cube
    // Otherwise, might have to do a more general approach. Currently ignoring the z-coordinate.
    return (-1 <= x_ && x_ <= 1) 
        && (-1 <= y_ && y_ <= 1);
}

Vertex Vertex::to_screen_coordinates(int xres, int yres) {
    double sx = (x_ + 1) * xres * 0.5;
    double sy = yres - (y_ + 1) * yres * 0.5;
    double sz = z_;
    double sw = w_;
    return Vertex(floor(sx), floor(sy), sz, sw);
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

vector<Vertex> Object::get_screen_coordinates(int xres, int yres) {
    // Initialize empty Vertex vector
    vector<Vertex> screen_vertices;
    screen_vertices.push_back(NULL_VERTEX);

    // For each NDC vertex, convert it to a screen vertex
    for(int i = 1; i < vertices.size(); i++) {
        screen_vertices.push_back(vertices[i].to_screen_coordinates(xres, yres));
    }

    return screen_vertices;
}

set<tuple<int, int>> Object::get_pixels(int xres, int yres) {
    // Initialize empty (x, y) set
    set<tuple<int,int>> pixels;

    for(int i = 0; i < faces.size(); i++) {
        // Obtains the face
        Face f = faces[i];

        // Obtains the vertices corresponding to the indices
        // in the Face object
        Vertex v1 = vertices[f.i1_];
        Vertex v2 = vertices[f.i2_];
        Vertex v3 = vertices[f.i3_];

        // Add the pixels necessary to draw lines between 
        // these 3 vertices pairwase using generalized Bresenham 
        // algorithm if the NDC coordinates are contained
        if (v1.is_contained() && v2.is_contained()) {
            Vertex p1 = v1.to_screen_coordinates(xres, yres);
            Vertex p2 = v2.to_screen_coordinates(xres, yres);
            generalized_bresenham(p1.x_, p1.y_, p2.x_, p2.y_, pixels);
        }

        if (v2.is_contained() && v3.is_contained()) {
            Vertex p2 = v2.to_screen_coordinates(xres, yres);
            Vertex p3 = v3.to_screen_coordinates(xres, yres);
            generalized_bresenham(p2.x_, p2.y_, p3.x_, p3.y_, pixels);
        }

        if (v1.is_contained() && v3.is_contained()) {
            Vertex p3 = v3.to_screen_coordinates(xres, yres);
            Vertex p1 = v1.to_screen_coordinates(xres, yres);
            generalized_bresenham(p1.x_, p1.y_, p3.x_, p3.y_, pixels); 
        }
    }

    return pixels;
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
        final_v.to_cartesian();
        transformed_vertices.push_back(final_v);
    }

    return transformed_vertices;
}

vector<Vertex> get_transformed_vertices(Object obj, Matrix4d transform) {
    vector<Vertex> transformed_vertices;
    transformed_vertices.push_back(NULL_VERTEX);

    // Applies transformation to all vertices of the object
    for (int i = 1; i < obj.vertices.size(); i++) {
        Vertex v = obj.vertices[i];
        Vector4d col = to_col_vector(v);
        Vector4d final = transform * col;
        Vertex final_v = to_vertex(final);
        final_v.to_cartesian();
        transformed_vertices.push_back(final_v);
    }

    return transformed_vertices;
}

