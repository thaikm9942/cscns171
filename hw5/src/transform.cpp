#include "../include/transform.h"
#include <math.h>

//////////////////////////////
///     CLASS FUNCTIONS    ///
//////////////////////////////

void Transform::print_transformation() {
    switch(type) {
        case TRANSLATION:
            printf("t: %f %f %f\n", parameters[0], parameters[1], parameters[2]);
            break;
        case ROTATION:
            printf("r: %f %f %f %f\n", parameters[0], parameters[1], parameters[2], angle);
            break;
        case SCALING:
            printf("s: %f %f %f\n", parameters[0], parameters[1], parameters[2]);
            break;
    }
}

void Transform_Set::add_rotation(float x, float y, float z, float rad) {
    transform_set.push_back(Transform(x, y, z, rad, ROTATION));
}

void Transform_Set::add_translation(float x, float y, float z) {
    transform_set.push_back(Transform(x, y, z, TRANSLATION));
}

void Transform_Set::add_scaling(float x, float y, float z) {
    transform_set.push_back(Transform(x, y, z, SCALING));
}

void Transform_Set::print_transform_set() {
    for (size_t i = 0; i < transform_set.size(); i++) {
        transform_set[i].print_transformation();
    }
}
