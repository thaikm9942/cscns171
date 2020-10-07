#include <stdlib.h>
#include <map>
#include <iostream>
#include <fstream>
#include <math.h>
#include "../include/transform.h"
#include "../include/object.h"

// A struct containing an obj and its associated label
struct Labeled_Object {
    string label;
    Object obj;
};

// Convert a Vertex to a 4 x 1 column vector
Vector4d to_col_vector(Vertex v) {
    Vector4d c;
    c << v.x_, v.y_, v.z_, v.w_;
    return c;
}

// Convert a 4 x 1 column vector to a Vertex
Vertex to_vertex(Vector4d col) {
    return Vertex(col[0], col[1], col[2], col[3]);
}

/* Default NULL_VERTEX to mark the start of a vector of Vertex objects
 * since vertices are 1-indexed
 */
const Vertex NULL_VERTEX = Vertex(INFINITY, INFINITY, INFINITY);

// String splitting function using vector<string> as a storage for the tokens
vector<string> strsplit(string &s, char delim) {
    vector<string> toks;
    istringstream iss(s);
    string item;
    while (getline(iss, item, delim)) {
        toks.push_back(item);
    }
    return toks;
}

// Create a corresponding transformation matrix from an opened
// filestream
Transformation create_transformation(ifstream& ifs) {
    string line;
    Transformation ts = Transformation();
    // If the line is empty, we have reached the end of the transformation block for the labeled
    // object, exit the function and return the transformation object
    while (getline(ifs, line) && !line.empty()) {
        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the type of the transformation
        char type = *(tokens[0].c_str());

        // If this is a rotation vector, has to read all 4 tokens in a row
        if (type == 'r') {
            double u[3] = {strtod(tokens[1].c_str(), NULL), strtod(tokens[2].c_str(), NULL), strtod(tokens[3].c_str(), NULL)};
            double theta = strtod(tokens[4].c_str(), NULL);
            ts.add_rotation(u, theta);
        }
        // Else, if it's either a translation or a scaling vector, than read only 3 tokens
        else if (type == 't' || type == 's') {
            double v[3] = {strtod(tokens[1].c_str(), NULL), strtod(tokens[2].c_str(), NULL), strtod(tokens[3].c_str(), NULL)};
            if (type == 't') 
            { ts.add_translation(v); }
            else 
            { ts.add_scaling(v); }
        }
        else {
            throw "Data does not contain either a translation, a rotation or a scaling vector\n";
        }
    }
    return ts;
}

// This function returns a graphical Object from a successfully
// opened file stream
Object create_object(ifstream &ifs) {
    // Initializes the vectors containing the corresponding vertices and faces
    // of an object
    Object obj = Object();
    obj.add(NULL_VERTEX);

    // Variables to store the tokens from the file
    // The "type" of data, either a face or a vertex
    char type;
    
    // The corrresponding vertex coordinates or vertex indices. We 
    // keep these as strings to convert to either floats or ints later
    string n1, n2, n3;

    // Process all tokens for either a vertex or a face line-by-line
    while (ifs >> type >> n1 >> n2 >> n3) {
        // If the first character of the line is 'v', then process the
        // strings as vertex float coordinates 
        if (type == 'v') {
            Vertex v = Vertex(strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL));
            obj.add(v);
        }
        // If the first character is 'f' instead, then process the strings
        // as the vertex integer indices
        else if (type == 'f') {
            Face f = Face(atoi(n1.c_str()), atoi(n2.c_str()), atoi(n3.c_str()));
            obj.add(f);
        }
        else {
            cout << "Data is neither a vertex or a file\n";
            break;
        }
    }
    return obj;
}

// Get object associated with a specific .obj file
Object get_object(const char* filename) {
    ifstream obj_ifs;
    
    // Opens a new read stream for the object file
    obj_ifs.open(filename, ifstream::in);

    // Returns the object data associated with the open stream
    return create_object(obj_ifs);
}

// Find and return (a copy of) the object with the correct label
Labeled_Object find_object_with_label(vector<Labeled_Object> elems, string label) 
{
    for (int i = 0; i < elems.size(); i++) {
        Labeled_Object elem = elems[i];
        if (elem.label.compare(label) == 0) {
            return elem;
        }
    }
    throw "Object with label not found";
}

// Print the output as formatted for all labeled objects
void print_transformations(vector<Labeled_Object> final_objs) {
    // Map to keep track of how many of the same objects there are
    map<string, int> label_map;
    for (int i = 0; i < final_objs.size(); i++) {
        Labeled_Object elem = final_objs[i];
        Object obj = elem.obj;

        if (label_map.find(elem.label) == label_map.end()) {
            label_map[elem.label] = 1;
        }
        else {
            label_map[elem.label] += 1;
        }

        // Prints the label and its copy number
        printf("%s_copy%d\n", elem.label.c_str(), label_map[elem.label]);

        // Applies transformation to all vertices of the object
        // and prints out the newly transformed coordinates
        for (int i = 1; i < obj.vertices.size(); i++) {
            Vertex v = obj.vertices[i];
            Vector4d col = to_col_vector(v);
            Vector4d final = obj.transform.compute_product() * col;
            Vertex final_v = to_vertex(final);
            cout << final_v.x_ << ' ' << final_v.y_ << ' ' << final_v.z_ << "\n";
        }

        // Newline formatting
        printf("\n");
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./my_prog <filename1.txt>\n");
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
            // A vector storing all the labeled objects without transformation
            vector<Labeled_Object> untransformed_objs;

            // A vector storing all the objects WITH the final transformation
            vector<Labeled_Object> final_objs;

            string line;
            while (getline(ifs, line)) {
                vector<string> tokens = strsplit(line, ' ');
                
                // If number of tokens are 2, then this line gives the name of an obj. file to be parsed.
                // Parse the file to obtain the associated object data 
                if (tokens.size() == 2) {
                    Labeled_Object l_obj = {tokens[0], get_object(tokens[1].c_str())};
                    untransformed_objs.push_back(l_obj);
                }

                // If there is only 1 token, then the next few lines containing the transformations
                // for the object with the corresponding label
                if (tokens.size() == 1) {
                    string label = tokens[0];

                    // Pass in the filestream to parse the transformations for the labeled object
                    Transformation t = create_transformation(ifs);
                    Labeled_Object elem = find_object_with_label(untransformed_objs, label);
                    elem.obj.transform = t;
                    final_objs.push_back(elem);
                }
            }

            // Pretty-print output according to assignment's format
            print_transformations(final_objs);
        }
    }
}