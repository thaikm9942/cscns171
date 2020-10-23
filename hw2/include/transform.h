#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <vector>
#include "../Eigen/Dense"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Eigen;

/* 
 * This header file defines the Transformation class and
 * the corresponding functions to create transformation
 * matrices
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class defines a transformation as a product of 
// rotations, translations and scaling in-order
class Transformation {
    vector<Matrix4d> ts_;
    public:
        // Default constructor that constructs an empty list of transformation matrices
        Transformation() : ts_() {}

        // Constructor that takes in a list of transformation matrices (in-order)
        // that defines the total transformation
        Transformation(vector<Matrix4d> ts) : ts_(ts) {}

        // Adds a CCW rotation to the transformation given a rotation vector and the
        // angle (in radians)
        void add_rotation(double u[3], double rad);

        // Adds a translation to the transformation given a vector
        void add_translation(double v[3]);

        // Adds a scaling to the transformation given a vector
        void add_scaling(double v[3]);

        // Computes the total transformation, which is the product of all transformations
        // by left-multiplying each transformation in order
        Matrix4d compute_product();

        // Print text representing the final transformation
        void print_transformation();
};

#endif // #ifndef __TRANSFORM_H__