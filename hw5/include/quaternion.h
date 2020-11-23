#ifndef __QUATERNION_H__
#define __QUATERNION_H__

/* 
 * This header file defines the Quaternion class, which is used to represent
 * rotations and allow the calculation of rotation matrices to less computationally
 * intensive
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

class Quaternion {
    public:
        // The real component
        float s;

        // The imaginary components i, j, k with real coefficients x, y, z
        float v[3];

        // Default constructor
        Quaternion() : s(0.0) {
            v[0] = 0.0, v[1] = 0.0, v[2] = 0.0;
        }

        // Constructor that takes in the real component and corresponding imaginary components
        Quaternion(float s, float x, float y, float z) : s(s) {
            v[0] = x, v[1] = y, v[2] = z;
        }

        // Add 2 Quaternions together and return their sum
        Quaternion add(Quaternion other);

        // Subtract the given Quaternion from the current one and return the difference
        Quaternion subtract(Quaternion other);

        // Multiply the current Quaternion with a scalar
        Quaternion scalar_dot(float c);

        // Multiply 2 Quaternions together and return their product
        Quaternion dot(Quaternion other);

        // Return the conjugate Quaternion of this Quaternion
        Quaternion conjugate();

        // Normalize this Quaternion
        void normalize();

        // Return the norm of this quartnernion
        float norm();
};

// Return the inverse of a given Quaternion
Quaternion inverse(Quaternion q);

// Converts a rotation matrix (using the unit vector axis and its angle of rotation) to a Quaternion object
Quaternion rot2quar(float u[3], float angle);

// Return the identity Quaternion (equivalent to the identity rotational matrix)
Quaternion Quaternion_identity();

#endif // #ifndef __QUATERNION_H__