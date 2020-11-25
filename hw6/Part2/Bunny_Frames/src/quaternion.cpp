#include "../include/quaternion.h"
#include <math.h>
#include <iostream>

using namespace std;

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
Quaternion Quaternion::add(Quaternion other) {
    return Quaternion(s + other.s, v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

Quaternion Quaternion::subtract(Quaternion other) {
    return Quaternion(s - other.s, v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

Quaternion Quaternion::scalar_dot(float c) {
    return Quaternion(s * c, v[0] * c, v[1] * c, v[2] * c);
}

Quaternion Quaternion::dot(Quaternion other) {
    // v_a * v_b
    float dot_v = v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];

    // Cross product of v_a and v_b
    float cx = v[1] * other.v[2] - v[2] * other.v[1];
    float cy = v[2] * other.v[0] - v[0] * other.v[2];
    float cz = v[0] * other.v[1] - v[1] * other.v[0];

    // [s_a * s_b - v_a * v_b, s_a * v_b + s_b * v_a + v_a x v_b
    return Quaternion(s * other.s - dot_v, 
        s * other.v[0] + other.s * v[0] + cx,
        s * other.v[1] + other.s * v[1] + cy, 
        s * other.v[2] + other.s * v[2] + cz);
}

Quaternion Quaternion::conjugate() {
    return Quaternion(s, -v[0], -v[1], -v[2]);
}

float Quaternion::norm() {
    return sqrt(s * s + v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Quaternion::normalize() {
    float norm = sqrt(s * s + v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    s = s / norm;
    v[0] = v[0] / norm;
    v[1] = v[1] / norm;
    v[2] = v[2] / norm;
}

//////////////////////////////
///    USEFUL FUNCTIONS    ///
//////////////////////////////

Quaternion inverse(Quaternion q) {
    Quaternion q_conj = q.conjugate();
    float squared_norm = q.norm() * q.norm();
    return q_conj.scalar_dot(1.0 / squared_norm);
}

Quaternion rot2quar(float u[3], float angle) {
    float c = sin(angle / 2);
    return Quaternion(cos(angle / 2), c * u[0], c * u[1], c * u[2]);
}

Quaternion Quaternion_identity() {
    return Quaternion(1.0, 0.0, 0.0, 0.0);
}

// Converts a Quaternion to a rotation matrix. The details are outlined in Lecture Notes for Assignment 3
Eigen::Matrix4f quar2rot(Quaternion q) {
    float qs = q.s;
    float qx = q.v[0];
    float qy = q.v[1];
    float qz = q.v[2];
    // Constructing the first row values
    float r11 = 1 - 2 * qy * qy - 2 * qz * qz;
    float r12 = 2 * (qx * qy - qz * qs);
    float r13 = 2 * (qx * qz + qy * qs);

    // Constructing the second row values
    float r21 = 2 * (qx * qy + qz * qs);
    float r22 = 1 - 2 * qx * qx - 2 * qz * qz;
    float r23 = 2 * (qy * qz - qx * qs);

    // Constructing the third row values
    float r31 = 2 * (qx * qz - qy * qs);
    float r32 = 2 * (qy * qz + qx * qs);
    float r33 = 1 - 2 * qx * qx - 2 * qy * qy;

    Eigen::Matrix4f r;
    r << r11, r12, r13, 0, // row 1
        r21, r22, r23, 0, // row 2
        r31, r32, r33, 0, // row 3
        0, 0, 0, 1; // row 4

    // Asserts that the product of a rotation matrix and its transpose 
    // should be the identity matrix
    assert((r * r.transpose()).isApprox(Eigen::Matrix4f::Identity(), 10e-7));
    
    return r;
}

void print_quaternion(Quaternion q) {
    cout << quar2rot(q) << endl;
}