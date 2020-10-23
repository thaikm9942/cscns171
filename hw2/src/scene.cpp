#include "../include/rasterization.h"

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////

Matrix4d Perspective::compute_perspective_matrix() {
    Matrix4d p;
    // Row 1
    double r11 = 2 * n_ / (r_ - l_);
    double r13 = (r_ + l_) / (r_ - l_);

    // Row 2
    double r22 = 2 * n_ / (t_ - b_);
    double r23 = (t_ + b_) / (t_ - b_);

    // Row 3
    double r33 = (-1) * (f_ + n_) / (f_ - n_);
    double r34 = (-2) * f_ * n_ / (f_ - n_);

    p << r11, 0, r13, 0, // row 1
         0, r22, r23, 0, // row 2
         0, 0, r33, r34, // row 3
         0, 0, -1, 0; // row 4
    return p;
}

void Perspective::print_perspective() {
    printf("perspective:\n");
    printf("n: %f, f: %f, l: %f, r: %f, t: %f, b: %f\n", n_, f_, l_, r_, t_, b_);
    printf("\n");
}


Transformation Camera::compute_camera_transform() {
    Transformation cam_t;
    cam_t.add_rotation(o_, angle_);
    cam_t.add_translation(p_);
    return cam_t;
}

void Camera::print_camera() {
    printf("camera:\n");
    printf("position: %f %f %f\n", p_[0], p_[1], p_[2]);
    printf("orientation: %f %f %f %f\n", o_[0], o_[1], o_[2], angle_);
    printf("\n");
}

void Scene::print_scene() {
    cam_.print_camera();
    persp_.print_perspective();
    for (int i = 0; i < objs_.size(); i++) {
        printf("%s\n", objs_[i].label.c_str());
        objs_[i].obj.print_object();
    }
    printf("\n");
    for (int i = 0; i < ls_.size(); i++) {
        printf("light%d\n", i);
        ls_[i].print_light();
    }
}

void Scene::add_labeled_object(Labeled_Object l_obj) {
    objs_.push_back(l_obj);
}

void Scene::add_labeled_object(Object obj, string label) {
    Labeled_Object l_obj = {label, obj};
    add_labeled_object(l_obj);
}

void Scene::apply_geometric_transforms() {
    for (int i = 0; i < objs_.size(); i++) {
        // Set the vertices of each object to be the new vertices
        // after applying all GEOMETRIC TRANSFORMATIONS to the object
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj);
    }
}

void Scene::apply_camera_transforms() {
    for (int i = 0; i < objs_.size(); i++) {
        // Convert all vertices from world space to camera space
        // by applying the inverse camera transform C^(-1) to
        // each vertex
        Matrix4d cam_transform = cam_.compute_camera_transform().compute_product();
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj, cam_transform.inverse());
    }
}

void Scene::apply_perspective_transform() {
    // Transform all vertices from camera space to NDC coordinates
    // by applying the perspective transform  
    for (int i = 0; i < objs_.size(); i++) {
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj, persp_.compute_perspective_matrix());
    }
}

void Scene::transform_normals() {
    for (int i = 0; i < objs_.size(); i++) {
        objs_[i].obj.vns_ = get_transformed_normals(objs_[i].obj);    
    }
}

void Scene::apply_transformations() {
    // Apply geometric transformations in world space coordinates
    
    apply_geometric_transforms();
    transform_normals();
}

void Scene::get_screen_coordinates() {
    // Iterate through all Labeled_Objects
    for (int i = 0; i < objs_.size(); i++) {
        objs_[i].obj.vs_ = objs_[i].obj.get_screen_coordinates(xres_, yres_);
    }
}

Vertex Scene::to_ndc_coordinates(Vertex v) {
    // Retrieve the camera transform and the perspective transform respectively
    Matrix4d cam_transform = cam_.compute_camera_transform().compute_product().inverse();
    Matrix4d perspective = persp_.compute_perspective_matrix();

    Vector4d col = to_col_vector(v);
    Vector4d final = perspective * cam_transform * col;
    Vertex final_v = to_vertex(final);
    final_v.to_cartesian();
    return final_v;
}

set<tuple<int, int>> Scene::get_pixels() {
    // Initialize empty set of (x, y) pixels for the Scene
    set<tuple<int, int>> pixels;

    // Iterate through a labeled objects
    for (int i = 0; i < objs_.size(); i++) {
        Object obj = objs_[i].obj;
        set<tuple<int, int>> obj_pixels = obj.get_pixels(xres_, yres_);
        pixels.insert(obj_pixels.begin(), obj_pixels.end());
    } 

    return pixels;
}

//////////////////////////////
///     MAIN FUNCTIONS     ///
//////////////////////////////

void scene_gourad_shading(Scene scene, vector<vector<Color>> &pixels, vector<vector<double>> &buffer) {
    for (int i = 0; i < scene.objs_.size(); i++) {
        Object obj = scene.objs_[i].obj;
        for (int j = 0; j < obj.fs_.size(); j++) {
            Face f = obj.fs_[j];
            Vertex a = obj.vs_[f.i_[0]];
            Vertex an = obj.vns_[f.n_[0]];
            Vertex b = obj.vs_[f.i_[1]];
            Vertex bn = obj.vns_[f.n_[1]];
            Vertex c = obj.vs_[f.i_[2]];
            Vertex cn = obj.vns_[f.n_[2]];
            gourad_shading(a, an, b, bn, c, cn, obj.m_, scene, pixels, buffer);
        }
    }
}