#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "./object.h"

/* 
 * This header file defines the Light class, which is used to represent
 * a light source, its color and attenuation, and the Color class, which
 * is used to represent rgb intensity values between 0 and 1
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class defines the position of the light source, its color and the attenuation constant
class Light {
    public:
        // Light position in world space
        float position[4];

        // RGB values of the light
        float color[3];

        // Attenuation parameter
        float attenuation_k;

        // Default constructor
        Light() : attenuation_k(0.0) {
            position[0] = 0.0, position[1] = 0.0, position[2] = 0.0, position[3] = 1.0;
            color[0] = 0.0, color[1] = 0.0, color[2] = 0.0;
        }

        // Constructor that takes in given parameters in homogenous coordinates
        Light(float p[4], float c[3], float k) : attenuation_k(k) {
            position[0] = p[0], position[1] = p[1], position[2] = p[2], position[3] = p[3];
            color[0] = c[0], color[1] = c[1], color[2] = c[2];
        }

        // Outputs the Light object as a string representation
        void print_light();
};

#endif // #ifndef __LIGHT_H__