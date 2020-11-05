#ifndef __QUARTERNION_H__
#define __QUARTERNION_H__

/* 
 * This header file defines the Quarternion class, which is used to represent
 * rotations and allow the calculation of rotation matrices to less computationally
 * intensive
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

class Quarternion {
    public:
        // The real component
        float s;

        // The imaginary components i, j, k with real coefficients x, y, z
        float v[3];

        // Default constructor
        Quarternion() : s(0.0) {
            v[0] = 0.0, v[1] = 0.0, v[2] = 0.0;
        }

        // Constructor that takes in the real component and corresponding imaginary components
        Quarternion(float s, float x, float y, float z) : s(s) {
            v[0] = x, v[1] = y, v[2] = z;
        }

        // Add 2 quarternions together and return their sum
        Quarternion add(Quarternion other);

        // Subtract the given quarternion from the current one and return the difference
        Quarternion subtract(Quarternion other);

        // Multiply the current quarternion with a scalar
        Quarternion scalar_dot(float c);

        // Multiply 2 quarternions together and return their product
        Quarternion dot(Quarternion other);

        // Return the conjugate quarternion of this quarternion
        Quarternion conjugate();

        // Normalize this quarternion
        void normalize();

        // Return the norm of this quartnernion
        float norm();
};

// Return the inverse of a given Quarternion
Quarternion inverse(Quarternion q);

// Converts a rotation matrix (using the unit vector axis and its angle of rotation) to a Quarternion object
Quarternion rot2quar(float u[3], float angle);

// Return the identity quarternion (equivalent to the identity rotational matrix)
Quarternion quarternion_identity();

#endif // #ifndef __QUARTERNION_H__