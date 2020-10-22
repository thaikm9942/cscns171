#include <iostream>
#include <fstream>
#include "../include/light.h"

//////////////////////////////
///     CLASS FUNCTIONS    ///
//////////////////////////////

void Light::print_light() {
    printf("light position: %f %f %f\n", l_[0], l_[1], l_[2]);
    printf("light rgb: %f %f %f\n", c_[0], c_[1], c_[2]);
    printf("attenuation: %f\n", k_);
}

void Material::print_material() {
    printf("ambient: %f %f %f\n", a_[0], a_[1], a_[2]);
    printf("diffusion: %f %f %f\n", d_[0], d_[1], d_[2]);
    printf("specular: %f %f %f\n", s_[0], s_[1], s_[2]);
    printf("shininess: %f\n", p_);
}