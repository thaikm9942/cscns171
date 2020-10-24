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

// This class represents a Color object using 3 integer values
class Color {  
    public:
        // The intensity values for the red, green, blue components
        // of the Color
        double r_, g_, b_;

        // Default constructor 
        Color() : r_(0.0), g_(0.0), b_(0.0) {}

        // Constructor that takes in given parameters
        Color(double r, double g, double b) : r_(r), g_(g), b_(b) {}
};


// This class defines the position of the light source, its color and the attenuation parameter
class Light {
    public:
        // Light position in world space, the RGB color of the light, and the attenuation parameter
        Vertex l_;
        Color c_;
        double k_;

        // Default constructor
        Light() : l_(), c_(), k_(0.0) {}

        // Constructor that takes in given parameters
        Light(double l[3], double c[3], double k) : 
            l_(l[0], l[1], l[2]), c_(c[0], c[1], c[2]), k_(k) {}

        // Outputs the Light object as a string representation
        void print_light();
};

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

/**
 * This function runs the lighting algorithm used to compute the color
 * of a point P on an illuminated surface and returns a Color
 * object representing the rgb color values of the point P.
 * 
 * @param p vertex whose color is to be determined
 * @param n the vertex normal associated with @param p
 * @param mat a pointer to the material properties of the object drawan
 * @param lights a pointer to the list of light sources
 * @param e an array of doubles corresponding to the camera's (x, y, z) coordinates in world space
 */
Color lighting(Vertex p, Vertex n, Material &mat, vector<Light> &lights, double e[3]);

#endif // #ifndef __LIGHT_H__