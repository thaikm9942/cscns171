#include "../include/transform.h"

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

/* This function constructs a CCW rotation matrix given a unit-vector u
* and an angle theta (in radians)
*/
Matrix4d create_rotation_matrix(double u[3], double theta) {
    Matrix4d r;
    // Constructing the first row values
    double r11 = u[0] * u[0] + (1 - u[0] * u[0]) * cos(theta);
    double r12 = u[0] * u[1] * (1 - cos(theta)) - u[2] * sin(theta);
    double r13 = u[0] * u[2] * (1 - cos(theta)) + u[1] * sin(theta);

    // Constructing the second row values
    double r21 = u[1] * u[0] * (1 - cos(theta)) + u[2] * sin(theta);
    double r22 = u[1] * u[1] + (1 - u[1] * u[1]) * cos(theta);
    double r23 = u[1] * u[2] * (1 - cos(theta)) - u[0] * sin(theta);

    // Constructing the third row values
    double r31 = u[2] * u[0] * (1 - cos(theta)) - u[1] * sin(theta);
    double r32 = u[2] * u[1] * (1 - cos(theta)) + u[0] * sin(theta);
    double r33 = u[2] * u[2] + (1 - u[2] * u[2]) * cos(theta);

    r << r11, r12, r13, 0, // row 1
        r21, r22, r23, 0, // row 2
        r31, r32, r33, 0, // row 3
        0, 0, 0, 1; // row 4

    // Asserts that the product of a rotation matrix and its transpose 
    // should be the identity matrix
    assert((r * r.transpose()).isApprox(Matrix4d::Identity(), 10e-7));
    return r;
}

// This function constructs a translation matrix from a given vector v
Matrix4d create_translation_matrix(double v[3]) {
    Matrix4d t;
    t << 1, 0, 0, v[0],
        0, 1, 0, v[1],
        0, 0, 1, v[2],
        0, 0, 0, 1;
    return t;
}

// This function constructs a scaling matrix from a given vector v
Matrix4d create_scaling_matrix(double v[3]) {
    Matrix4d s;
    s << v[0], 0, 0, 0,
        0, v[1], 0, 0,
        0, 0, v[2], 0,
        0, 0, 0, 1;
    return s;
}

//////////////////////////////
///     MEMBER FUNCTIONS   ///
//////////////////////////////

void Transformation::add_rotation(double u[3], double rad) {
    ts_.push_back(create_rotation_matrix(u, rad));
}

void Transformation::add_translation(double v[3]) {
    ts_.push_back(create_translation_matrix(v));
}

void Transformation::add_scaling(double v[3]) {
    ts_.push_back(create_scaling_matrix(v));
}

Matrix4d Transformation::compute_product() {
    // Initialize product as an identity matrix
    Matrix4d prod = Matrix4d::Identity();

    // Iterate through all transformations and left multiply them in order
    for (int i = 0; i < ts_.size(); i++) {
        prod = ts_[i] * prod;
    }

    return prod;
}

void Transformation::print_transformation() {
    Matrix4d prod = compute_product();
    cerr << prod << endl;
}