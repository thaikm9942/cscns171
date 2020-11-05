#include "../include/quarternion.h"
#include <math.h>

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
Quarternion Quarternion::add(Quarternion other) {
    return Quarternion(s + other.s, v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

Quarternion Quarternion::subtract(Quarternion other) {
    return Quarternion(s - other.s, v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

Quarternion Quarternion::scalar_dot(float c) {
    return Quarternion(s * c, v[0] * c, v[1] * c, v[2] * c);
}

Quarternion Quarternion::dot(Quarternion other) {
    // v_a * v_b
    float dot_v = v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];

    // Cross product of v_a and v_b
    float cx = v[1] * other.v[2] - v[2] * other.v[1];
    float cy = v[2] * other.v[0] - v[0] * other.v[2];
    float cz = v[0] * other.v[1] - v[1] * other.v[0];

    // [s_a * s_b - v_a * v_b, s_a * v_b + s_b * v_a + v_a x v_b
    return Quarternion(s * other.s - dot_v, 
        s * other.v[0] + other.s * v[0] + cx,
        s * other.v[1] + other.s * v[1] + cy, 
        s * other.v[2] + other.s * v[2] + cz);
}

Quarternion Quarternion::conjugate() {
    return Quarternion(s, -v[0], -v[1], -v[2]);
}

float Quarternion::norm() {
    return sqrt(s * s + v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Quarternion::normalize() {
    float norm = sqrt(s * s + v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    s = s / norm;
    v[0] = v[0] / norm;
    v[1] = v[1] / norm;
    v[2] = v[2] / norm;
}

Quarternion inverse(Quarternion q) {
    Quarternion q_conj = q.conjugate();
    float squared_norm = q.norm() * q.norm();
    return q_conj.scalar_dot(1.0 / squared_norm);
}

Quarternion rot2quar(float u[3], float angle) {
    float c = sin(angle / 2);
    return Quarternion(cos(angle / 2), c * u[0], c * u[1], c * u[2]);
}

Quarternion quarternion_identity() {
    return Quarternion(1.0, 0.0, 0.0, 0.0);
}