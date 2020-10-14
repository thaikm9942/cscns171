#include <string.h>
#include "../include/parser.h"

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

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

//////////////////////////////
///    MAIN FUNCTIONS      ///
//////////////////////////////

// Get object associated with a specific .obj file
Object get_object(const char* filename) {
    ifstream obj_ifs;
    
    // Opens a new read stream for the object file
    obj_ifs.open(filename, ifstream::in);

    // Returns the object data associated with the open stream
    return create_object(obj_ifs);
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