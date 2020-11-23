#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

// Enums defining the types of transformation
enum transformation_type {
    TRANSLATION = 1,
    ROTATION = 2,
    SCALING = 3,
};

/* 
 * This header file defines the Transform class (which defines a type of geoemtric transformation)
 * and the Transforms class (which defines a single set of geometric transformation)
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class defines a transformation given the parameters (x, y, z), an angle (applicable only if the
// transformation is a rotation) and the transformation type
class Transform {
    public:
        // The x, y, z coordinates
        float parameters[3];

        // The angle of rotation (only applicable for rotational matrices)
        float angle;

        // Type of transform
        transformation_type type;

        // Constructor for a translational or scaling matrix
        Transform(float x, float y, float z, transformation_type type) : angle(0.0), type(type) {
            parameters[0] = x, parameters[1] = y, parameters[2] = z;
        }

        // Constructor for a rotational matrix
        Transform(float x, float y, float z, float angle, transformation_type type) : angle(angle), type(type) {
            parameters[0] = x, parameters[1] = y, parameters[2] = z;
        }

        // Print text representing a geometric transformation and its type
        void print_transformation();
};

// This class defines a single SET of transformations (which includes rotations, translations and scalings).
// The geometric transformations are added depending on the order of which they are parsed.
class Transform_Set {
        public:
        // A vector storing a set of geometric transformations
        vector<Transform> transform_set;

        // Default constructor
        Transform_Set() : transform_set() {};

        // Adds a CCW rotation to the transformation given the (x, y, z)-coordinates of a rotation vector and the
        // angle (in radians)
        void add_rotation(float x, float y, float z, float rad);

        // Adds a translation transformation given the x, y, z parameters
        void add_translation(float x, float y, float z);

        // Adds a scaling transformation given the x, y, z parameters
        void add_scaling(float x, float y, float z);

        // Print text representing a set of geometric transformation
        void print_transform_set();
};

#endif // #ifndef __TRANSFORM_H__