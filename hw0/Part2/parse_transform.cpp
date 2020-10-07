#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "../include/transform.h"

// Creates a corresponding transformation matrix from an opened
// filestream
Matrix4d create_transformation_matrix(ifstream& ifs) {
    char type;
    string n1, n2, n3, n4;
    Transformation ts = Transformation();

    while (ifs >> type) {
        // If this is a rotation vector, has to read all 4 tokens in a row
        if (type == 'r') {
            ifs >> n1 >> n2 >> n3 >> n4;
            
            double u[3] = {strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL)};
            double theta = strtod(n4.c_str(), NULL);
            ts.add_rotation(u, theta);
        }
        // Else, if it's either a translation or a scaling vector, than read only 3 tokens
        else if (type == 't' || type == 's') {
            ifs >> n1 >> n2 >> n3;
            double v[3] = {strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL)};
            if (type == 't') { ts.add_translation(v); }
            else { ts.add_scaling(v); }
        }
        else {
            throw "Data does not contain either a translation, a rotation or a scaling vector\n";
            break;
        }
    }
    return ts.compute_product();
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
            throw "Error opening file\n";
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