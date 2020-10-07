#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "object.h"

/* Default NULL_VERTEX to mark the start of a vector of Vertex objects
 * since vertices are 1-indexed
 */
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

// Prints in the object to the output in a formatted paragraph
void print_object(Object obj) {
    // Enumerates all the vertices in the object
    for (int i = 1; i < obj.vertices.size(); i++) {
        Vertex v = obj.vertices[i];
        printf("v %f %f %f\n", v.x_, v.y_, v.z_);
    }
    // Enumerates all the faces in the object
    for (int i = 0; i < obj.faces.size(); i++) {
        Face f = obj.faces[i];
        printf("f %d %d %d\n", f.i1_, f.i2_, f.i3_);
    }
}

// This function returns a graphical Object from a succesfully
// opened file stream
Object create_object(ifstream &ifs) {
    // Initializes the vectors containing the corresponding vertices and faces
    // of an object
    Object obj = Object();
    obj.add_vertex(NULL_VERTEX);

    // Variables to store the tokens from the file
    // The "type" of data, either a face or a vertex
    char type;
    
    // The corrresponding vertex coordinates or vertex indices. We 
    // keep these as strings to convert to either floats or ints later
    string n1, n2, n3;

    // Proceeds all tokens for either a vertex or a face line-by-line
    while (ifs >> type >> n1 >> n2 >> n3) {
        // If the first character of the line is 'v', then process the
        // strings as vertex float coordinates 
        if (type == 'v') {
            Vertex v = Vertex(strtof(n1.c_str(), NULL), strtof(n2.c_str(), NULL), strtof(n3.c_str(), NULL));
            obj.add_vertex(v);
        }
        // If the first character is 'f' instead, then process the strings
        // as the vertex integer indices
        else if (type == 'f') {
            Face f = Face(atoi(n1.c_str()), atoi(n2.c_str()), atoi(n3.c_str()));
            obj.add_face(f);
        }
        else {
            cout << "Data is neither a vertex or a file\n";
            break;
        }
    }
    return obj;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./parse <filename1.obj> <filename2.obj> ...\n";
    }
    else {
        // Iterate through all object
        for (size_t i = 1; i < argc; i++) {
            // Initialize filestream
            ifstream ifs;

            // Open the file
            ifs.open(argv[i], ifstream::in);
            // If file opening is unsuccessful, print out an error message
            if (!ifs.is_open()) {
                cout << "Error opening file\n";
            }
            else {      
                // Use the opened filestream to create a new object  
                Object obj = create_object(ifs);               
                
                // Print the .obj using the desired format
                printf("%s\n", argv[i]);
                print_object(obj);
                printf("\n");        
            }
        }
    }
}