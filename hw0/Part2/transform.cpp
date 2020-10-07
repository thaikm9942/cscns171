#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "transform.h"

using namespace std;

/* This function constructs a CCW rotation matrix given a unit-vector u
 * and an angle theta (in radians)
 */
Matrix4d create_rotation_matrix(double u[3], double theta) {
    Matrix4d r;
    // Constructing the first row values
    double r11 = u[0] * u[0] + (1 - u[0] * u[0]) * cos(theta);
    double r12 = u[0] * u[1] * (1 - cos(theta)) - u[2] * sin(theta);
    double r13 = u[0] * u[2] * (1 - cos(theta)) + u[1] * sin(theta);

    // Constructing the second row values
    double r21 = u[1] * u[0] * (1 - cos(theta)) + u[2] * sin(theta);
    double r22 = u[1] * u[1] + (1 - u[1] * u[1]) * cos(theta);
    double r23 = u[1] * u[2] * (1 - cos(theta)) - u[0] * sin(theta);

    // Constructing the third row values
    double r31 = u[2] * u[0] * (1 - cos(theta)) - u[1] * sin(theta);
    double r32 = u[2] * u[1] * (1 - cos(theta)) + u[0] * sin(theta);
    double r33 = u[2] * u[2] + (1 - u[2] * u[2]) * cos(theta);

    r << r11, r12, r13, 0.0, // row 1
         r21, r22, r23, 0.0, // row 2
         r31, r32, r33, 0, // row 3
         0, 0, 0, 1; // row 4

    // Asserts that the product of a rotation matrix and its transpose 
    // should be the identity matrix
    assert((r * r.transpose()).isApprox(Matrix4d::Identity(), 10e-7));
    return r;
}

// This function constructs a translation matrix from a given vector v
Matrix4d create_translation_matrix(double v[3]) {
    Matrix4d t;
    t << 1, 0, 0, v[0],
         0, 1, 0, v[1],
         0, 0, 1, v[2],
         0, 0, 0, 1;
    return t;
}

// This function constructs a scaling matrix from a given vector v
Matrix4d create_scaling_matrix(double v[3]) {
    Matrix4d s;
    s << v[0], 0, 0, 0,
         0, v[1], 0, 0,
         0, 0, v[2], 0,
         0, 0, 0, 1;
    return s;
}

// Creates a corresponding transformation matrix from an opened
// filestream
Matrix4d create_transformation_matrix(ifstream& ifs) {
    char type;
    string n1, n2, n3, n4;
    vector<Matrix4d> transformations;

    while (ifs >> type) {
        // If this is a rotation vector, has to read all 4 tokens in a row
        if (type == 'r') {
            ifs >> n1 >> n2 >> n3 >> n4;
            
            double u[3] = {strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL)};
            double theta = strtod(n4.c_str(), NULL);
            Matrix4d r = create_rotation_matrix(u, theta);
            transformations.push_back(r);
        }
        // Else, if it's either a translation or a scaling vector, than read only 3 tokens
        else if (type == 't' || type == 's') {
            ifs >> n1 >> n2 >> n3;
            double v[3] = {strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL)};
            Matrix4d m;
            if (type == 't') { m = create_translation_matrix(v); }
            else { m = create_scaling_matrix(v); }
            transformations.push_back(m);
        }
        else {
            cout << "Data does not contain either a translation, a rotation or a scaling vector\n";
            break;
        }
    }

    // Initialize an identity matrix to be used to compute the product of all transformation matrices
    Matrix4d product = Matrix4d::Identity();

    // While there are transformations, left-multiply the current product with the LAST element in
    // the vector 
    while (!transformations.empty()) {
        Matrix4d transformation = transformations.back();     
        product = transformation * product;
        transformations.pop_back();
    }

    return product;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./transform <filename1.txt>\n");
    }
    else {
        // Initialize filestream
        ifstream ifs;

        // Open the text file
        ifs.open(argv[1], ifstream::in);

        // If file opening is unsuccessful, print out an error message
        if (!ifs.is_open()) {
            cout << "Error opening file\n";
        }
        else {      
            // Use the opened filestream to create a new transformation matrix
            Matrix4d product = create_transformation_matrix(ifs); 

            // Finds the inverse of the product of the transformation matrices
            Matrix4d inverse = product.inverse();

            // Outputs the inverse matrix
            cout << "Inverse of product of transformation matrix:\n";
            cout << inverse << endl;
        }
    }
}