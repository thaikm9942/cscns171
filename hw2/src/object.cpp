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

void Vertex::normalize() {
    double norm = sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    x_ = x_ / norm;
    y_ = y_ / norm;
    z_ = z_ / norm;
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
    return Vertex(round(sx), round(sy), sz, sw);
}

void Face::print_face() {
    printf("v%d v%d v%d\n", i_[0], i_[1], i_[2]);
    printf("vn%d vn%d vn%d\n", n_[0], n_[1], n_[2]);
    printf("\n");
}

void Object::add_normal(Vertex vn) {
    vns_.push_back(vn);
}

void Object::add(Vertex v) { 
    vs_.push_back(v); 
}

void Object::add(Face f) { 
    fs_.push_back(f); 
}

Vertex Object::get_vertex(int i) {
    return vs_[i];
}

Vertex Object::get_vertex_normal(int in) {
    return vns_[in];
}

void Object::print_object() {
    printf("vertices:\n");
    for(int i = 0; i < vs_.size(); i++) {
        vs_[i].print_vertex();
    }

    printf("normals:\n");
    for(int i = 0; i < vs_.size(); i++) {
        vns_[i].print_vertex();
    }

    printf("faces:\n");
    for(int i = 0; i < fs_.size(); i++) {
        fs_[i].print_face();
    }

    printf("transformation:\n");
    t_.print_transformation();
    printf("\n");

    printf("material:\n");
    m_.print_material();
}

vector<Vertex> Object::get_screen_coordinates(int xres, int yres) {
    // Initialize empty Vertex vector
    vector<Vertex> screen_vertices;
    screen_vertices.push_back(NULL_VERTEX);

    // For each NDC vertex, convert it to a screen vertex
    for(int i = 1; i < vs_.size(); i++) {
        screen_vertices.push_back(vs_[i].to_screen_coordinates(xres, yres));
    }

    return screen_vertices;
}

set<tuple<int, int>> Object::get_pixels(int xres, int yres) {
    // Initialize empty (x, y) set
    set<tuple<int,int>> pixels;

    for(int i = 0; i < fs_.size(); i++) {
        // Obtains the face
        Face f = fs_[i];

        // Obtains the vertices corresponding to the indices
        // in the Face object
        Vertex v1 = vs_[f.i_[0]];
        Vertex v2 = vs_[f.i_[1]];
        Vertex v3 = vs_[f.i_[2]];

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
///    MAIN FUNCTIONS      ///
//////////////////////////////

// Calculate the vector resulting from multiplying a scalar with a given vector
Vertex dot(double scalar, Vertex v) {
    return Vertex(scalar * v.x_, scalar * v.y_, scalar * v.z_);
}

// Calculate the dot product between 2 vectors
double dot(Vertex v1, Vertex v2) {
    return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
}

// Calculate the cross product between 2 vectors 
Vertex cross(Vertex v1, Vertex v2) {
    double cx = v1.y_ * v2.z_ - v1.z_ * v2.y_;
    double cy = v1.z_ * v2.x_ - v1.x_ * v2.z_;
    double cz = v1.x_ * v2.y_ - v1.y_ * v2.x_;
    return Vertex(cx, cy, cz);
}

// Add 2 vertices together to obtain a new vertex
Vertex add(Vertex v1, Vertex v2) {
    double new_x = v1.x_ + v2.x_;
    double new_y = v1.y_ + v2.y_;
    double new_z = v1.z_ + v2.z_;
    return Vertex(new_x, new_y, new_z);
}

// Compute the distance squared between 2 vertices
double compute_distance_squared(Vertex v1, Vertex v2) {
    double dx = v2.x_ - v1.x_;
    double dy = v2.y_ - v1.y_;
    double dz = v2.z_ - v1.z_;
    return dx * dx + dy * dy + dz * dz;
}

// Subtracts one vertex from another vertex to obtain a new vertex
Vertex subtract(Vertex v1, Vertex v2) {
    double new_x = v1.x_ - v2.x_;
    double new_y = v1.y_ - v2.y_;
    double new_z = v1.z_ - v2.z_;  
    return Vertex(new_x, new_y, new_z);
}

// Remove the translation components from the final transform
Matrix4d make_normal_transform(Matrix4d transform) {
    for(int row = 0; row < 3; row++) {
        transform(row, 3) = 0.0;
    }
    return transform;
}
vector<Vertex> get_transformed_normals(Object obj) {
    vector<Vertex> transformed_normals;
    transformed_normals.push_back(NULL_VERTEX);

    // Applies transformation to all vertex normals of the Object
    for (int i = 1; i < obj.vns_.size(); i++) {
        Vertex vn = obj.vns_[i];
        Vector4d col = to_col_vector(vn);
        Matrix4d transform = obj.t_.compute_product();
        make_normal_transform(transform);
        Matrix4d inv = transform.inverse();
        Vector4d final = inv.transpose() * col;
        Vertex final_vn = to_vertex(final);
        final_vn.normalize();
        transformed_normals.push_back(final_vn);
    } 
    return transformed_normals;
}

vector<Vertex> get_transformed_vertices(Object obj) {
    vector<Vertex> transformed_vertices;
    transformed_vertices.push_back(NULL_VERTEX);

    // Applies transformation to all vertices of the object
    for (int i = 1; i < obj.vs_.size(); i++) {
        Vertex v = obj.vs_[i];
        Vector4d col = to_col_vector(v);
        Vector4d final = obj.t_.compute_product() * col;
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
    for (int i = 1; i < obj.vs_.size(); i++) {
        Vertex v = obj.vs_[i];
        Vector4d col = to_col_vector(v);
        Vector4d final = transform * col;
        Vertex final_v = to_vertex(final);
        final_v.to_cartesian();
        transformed_vertices.push_back(final_v);
    }

    return transformed_vertices;
}

