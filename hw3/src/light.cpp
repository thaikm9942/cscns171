#include <iostream>
#include <fstream>
#include "../include/light.h"
#include <math.h>

//////////////////////////////
///     CLASS FUNCTIONS    ///
//////////////////////////////

void Light::print_light() {
    printf("light position: %f %f %f %f\n", position[0], position[1], position[2], position[3]);
    printf("light rgb: %f %f %f\n", color[0], color[1], color[2]);
    printf("attenuation: %f\n", attenuation_k);
    printf("\n");
}

void Material::print_material() {
    printf("ambient: %f %f %f\n", ambient[0], ambient[1], ambient[2]);
    printf("diffusion: %f %f %f\n", diffuse[0], diffuse[1], diffuse[2]);
    printf("specular: %f %f %f\n", specular[0], specular[1], specular[2]);
    printf("shininess: %f\n", shininess);
}