#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "./pixel.h"

/* 
 * This header file defines the Light class, which is used to represent
 * a light source, its color and attenuation, and the Material class,
 * which is used to represent the different reflectance properties and 
 * the shininess of an object
 */

// This class defines the position of the light source, its color and the attenuation parameter
class Light {
    // Light position in world space, the RGB color of the light, and the attenuation parameter
    double l_[3];
    double c_[3];
    double k_;

    public:
        // Default constructor
        Light() : k_(0.0) {
            l_[0] = 0.0, l_[1] = 0.0, l_[2] = 0.0;
            c_[0] = 0.0, c_[1] = 0.0, c_[2] = 0.0;        
        }

        // Constructor that takes in given parameters
        Light(double l[3], double c[3], double k) : k_(k) {
            l_[0] = l[0], l_[1] = l[1], l_[2] = l[2];
            c_[0] = c[0], c_[1] = c[1], c_[2] = c[2];           
        }

        // Outputs the Light object as a string representation
        void print_light();
};

// This class defines different properties of an Object, including ambient material reflectance,
// diffuse material reflectance, specular material reflectance, and material "shininess"
class Material {
    // Ambient material reflectance
    double a_[3];
    // Diffuse material reflectance
    double d_[3];
    // Specular material reflectance
    double s_[3];
    // Shininess of the object (or Phong exponent)
    double p_;

    public:
        // Default constructor
        Material() : p_(0.0) { 
            a_[0] = 0.0, a_[1] = 0.0, a_[2] = 0.0;
            d_[0] = 0.0, d_[1] = 0.0, d_[2] = 0.0;
            s_[0] = 0.0, s_[1] = 0.0, s_[2] = 0.0;
        }

        // Constructor that takes in given parameters.
        Material(double a[3], double d[3], double s[3], double p) : p_(p) {
            a_[0] = a[0], a_[1] = a[1], a_[2] = a[2];
            d_[0] = d[0], d_[1] = d[1], d_[2] = d[2];
            s_[0] = s[0], s_[1] = s[1], s_[2] = s[2];
        }

        // Outputs the Material object as a string representation
        void print_material();
};


#endif // #ifndef __LIGHT_H__