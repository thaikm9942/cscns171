#include "../include/scene.h"

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
        objs_[i].obj.m_.print_material();
    }
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

void Scene::apply_transformations() {
    // For each object, convert all world space vertices to NDC coordinates
    for (int i = 0; i < objs_.size(); i++) {
        // Set the vertices of each object to be the new vertices
        // after applying all GEOMETRIC TRANSFORMATIONS to the object
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj);

        // Convert all vertices from world space to camera space
        // by applying the inverse camera transform C^(-1) to
        // each vertex
        Matrix4d cam_transform = cam_.compute_camera_transform().compute_product();
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj, cam_transform.inverse());

        // Transform all vertices from camera space to NDC coordinates
        // by applying the perspective transform  
        objs_[i].obj.vs_ = get_transformed_vertices(objs_[i].obj, persp_.compute_perspective_matrix());
    }
}

void Scene::get_screen_coordinates(int xres, int yres) {
    // Iterate through all Labeled_Objects
    for (int i = 0; i < objs_.size(); i++) {
        objs_[i].obj.vs_ = objs_[i].obj.get_screen_coordinates(xres, yres);
    }
}

set<tuple<int, int>> Scene::get_pixels(int xres, int yres) {
    // Initialize empty set of (x, y) pixels for the Scene
    set<tuple<int, int>> pixels;

    // Iterate through a labeled objects
    for (int i = 0; i < objs_.size(); i++) {
        Object obj = objs_[i].obj;
        set<tuple<int, int>> obj_pixels = obj.get_pixels(xres, yres);
        pixels.insert(obj_pixels.begin(), obj_pixels.end());
    } 

    return pixels;
}