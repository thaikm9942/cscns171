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


Transformation Camera::compute_camera_matrix() {
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
    perp_.print_perspective();
    for(int i = 0; i < objs_.size(); i++) {
        printf("%s\n", objs_[i].label.c_str());
        objs_[i].obj.print_object();
    }
}

void Scene::add_labeled_object(Labeled_Object l_obj) {
    objs_.push_back(l_obj);
}

void Scene::add_labeled_object(Object obj, string label) {
    Labeled_Object l_obj = {label, obj};
    add_labeled_object(l_obj);
}

