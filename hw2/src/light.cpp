#include <iostream>
#include <fstream>
#include "../include/light.h"
#include <math.h>

//////////////////////////////
///     CLASS FUNCTIONS    ///
//////////////////////////////

void Light::print_light() {
    printf("light position: %f %f %f\n", l_.x_, l_.y_, l_.z_);
    printf("light rgb: %f %f %f\n", c_.r_, c_.g_, c_.b_);
    printf("attenuation: %f\n", k_);
    printf("\n");
}

void Material::print_material() {
    printf("ambient: %f %f %f\n", a_[0], a_[1], a_[2]);
    printf("diffusion: %f %f %f\n", d_[0], d_[1], d_[2]);
    printf("specular: %f %f %f\n", s_[0], s_[1], s_[2]);
    printf("shininess: %f\n", p_);
}

//////////////////////////////
///     MAIN FUNCTIONS     ///
//////////////////////////////

// Implements the lighting algorithm
Color lighting(Vertex p, Vertex n, Material mat, vector<Light> lights, double e[3]) {
    Color cp;

    // Initializes the direction vector pointing from p to the camera
    Vertex e_dir = Vertex(e[0] - p.x_, e[1] - p.y_, e[2] - p.z_);
    e_dir.normalize();

    // Initializes the sum of diffusion reflectance and specular
    // reflectance
    double diff_sum[3] = {0, 0, 0};
    double spec_sum[3] = {0, 0, 0};

    for (int i = 0; i < lights.size(); i++) {
        // Retrieves the position and color value of the light source
        Vertex l = lights[i].l_;
        Color c = lights[i].c_;

        // Calculates the attenuation value
        double att = 1.0 / (1.0 + lights[i].k_ * compute_distance_squared(l, p));
        
        // Reduces the color value for the light source by the attenuation value
        c.r_ *= att;
        c.g_ *= att;
        c.b_ *= att;

        // The vector pointing from p to the light source
        Vertex l_dir = Vertex(l.x_ - p.x_, l.y_ - p.y_, l.z_ - p.z_);
        l_dir.normalize();
        // printf("n: %f %f %f\n", n.x_, n.y_, n.z_);

        // Calculates the diffuse factor
        // cout << "dot product: " << dot(n, l_dir) << "\n";
        double diffuse_factor = max(0.0, dot(n, l_dir));
        // cout << "diff_fact: " << diffuse_factor << "\n";
        diff_sum[0] += diffuse_factor * c.r_;
        diff_sum[1] += diffuse_factor * c.g_;
        diff_sum[2] += diffuse_factor * c.b_;
        // printf("diff_sum: %f %f %f\n", diff_sum[0], diff_sum[1], diff_sum[2]);
        // Obtain the h vector that bisects l_dir and e_dir
        Vertex h = add(e_dir, l_dir);
        h.normalize();
        // h.print_vertex();

        // Calculates the specular factor, taken into account the shininess (or Phong's exponent)
        double specular_factor = pow(max(0.0, dot(n, h)), mat.p_);
        spec_sum[0] += specular_factor * c.r_;
        spec_sum[1] += specular_factor * c.g_;
        spec_sum[2] += specular_factor * c.b_;
    }

    // Sets the color values of the point P to be the componentwise minimum of
    // [1, 1 ,1] and the sum of the material's ambient reflectance, the componentwise product
    // of the diffusion sum and the material's diffusion reflectance, and the componentwise
    // product of the specular sum and the material's specular reflectance
    cp.r_ = min(1.0, mat.a_[0] + diff_sum[0] * mat.d_[0] + spec_sum[0] * mat.s_[0]);
    cp.g_ = min(1.0, mat.a_[1] + diff_sum[1] * mat.d_[1] + spec_sum[1] * mat.s_[1]);
    cp.b_ = min(1.0, mat.a_[2] + diff_sum[2] * mat.d_[2] + spec_sum[2] * mat.s_[2]);
    // printf("cp: %f %f %f\n", cp.r_, cp.g_, cp.b_);
    return cp;
}