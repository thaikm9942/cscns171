#include <string.h>
#include "../include/parser.h"

using namespace std;

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

// This function returns a graphical Object from a successfully
// opened file stream
Object parse_object(ifstream &ifs) {
    // Initializes the vectors containing the corresponding vertices and faces
    // of an object
    Object obj = Object();
    obj.add(NULL_VERTEX);

    // Variables to store the tokens from the file
    // The "type" of data, either a face or a vertex
    string type;
    
    // The corresponding vertex coordinates or vertex indices. We 
    // keep these as strings to convert to either floats or ints later
    string n1, n2, n3;

    // Process all tokens for either a vertex or a face line-by-line
    while (ifs >> type >> n1 >> n2 >> n3) {
        // If the first character of the line is 'v', then process the
        // strings as vertex float coordinates 
        if (type.compare(string("v")) == 0) {
            Vertex v = Vertex(strtof(n1.c_str(), NULL), strtof(n2.c_str(), NULL), strtof(n3.c_str(), NULL));
            obj.add(v);
        }
        // If the first character is 'f' instead, then process the strings
        // as vertex indices of a face
        else if (type.compare(string("f")) == 0) {
            // Indices of the vertex in the face
            int idx1 = atoi(n1.c_str());
            int idx2 = atoi(n2.c_str());
            int idx3 = atoi(n3.c_str());

            // Create Face object and add Face to the Object
            Face f = Face(idx1, idx2, idx3);
            obj.add_face(f);
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

// Get object associated with a specific .obj file
Object create_object(const char* filename) {
    ifstream obj_ifs;
    
    // Opens a new read stream for the object file
    obj_ifs.open(filename, ifstream::in);

    Object obj;
    if (!obj_ifs.is_open()) {
        throw "Error opening .obj file\n";
    }
    else {
        obj = parse_object(obj_ifs);
    }

    // Returns the object data associated with the open stream
    return obj;
}

vector<Light> create_lights(ifstream &ifs) {
    string line;
    
    // Initialize storage for all lights
    vector<Light> ls;

    while (getline(ifs, line) && !line.empty()) {
        // Initializes storage for all light source information
        float l[4], c[3];
        float k;

        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the info type
        string info_type = tokens[0];

        if (info_type.compare(string("light")) == 0) {
            l[0] = strtof(tokens[1].c_str(), NULL);
            l[1] = strtof(tokens[2].c_str(), NULL); 
            l[2] = strtof(tokens[3].c_str(), NULL);
            l[3] = 1.0;

            // Skips 1 to account for commas
            c[0] = strtof(tokens[5].c_str(), NULL);
            c[1] = strtof(tokens[6].c_str(), NULL); 
            c[2] = strtof(tokens[7].c_str(), NULL);

            // Skips 1 again to account for commas
            k = strtof(tokens[9].c_str(), NULL);
            
            // Create a Light object to be added to our list of lights
            ls.push_back(Light(l, c, k));
        }
        else {
            throw "Text block does not contain light source data";
        }
    }
    return ls;
}

Material create_material(ifstream &ifs) {
    string line;
    Material m;

    // Initializes storage for all material properties
    float a[3], d[3], s[3];
    float p;

    while (getline(ifs, line) && !line.empty()) {
        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the info type
        string info_type = tokens[0];

        // Parses information based on the information type
        if (info_type.compare(string("ambient")) == 0) {
            a[0] = strtof(tokens[1].c_str(), NULL);
            a[1] = strtof(tokens[2].c_str(), NULL); 
            a[2] = strtof(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("diffuse")) == 0) {
            d[0] = strtof(tokens[1].c_str(), NULL);
            d[1] = strtof(tokens[2].c_str(), NULL); 
            d[2] = strtof(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("specular")) == 0) {
            s[0] = strtof(tokens[1].c_str(), NULL);
            s[1] = strtof(tokens[2].c_str(), NULL); 
            s[2] = strtof(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("shininess")) == 0) {
            p = strtof(tokens[1].c_str(), NULL);
            // Break out of the current stream to move on to reading
            // the transformation block of text
            break;
        }
        else {
            throw "Text block does not contain ambient, diffuse, specular reflectance or shininess";
        }
    }

    m = Material(a, d, s, p);
    return m;
}

// Create a corresponding transformation matrix from an opened
// filestream
Transform_Set create_transformation(ifstream& ifs) {
    string line;
    Transform_Set ts = Transform_Set();
    // If the line is empty, we have reached the end of the transformation block for the labeled
    // object, exit the function and return the transformation object
    while (getline(ifs, line) && !line.empty()) {
        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the type of the transformation
        char type = *(tokens[0].c_str());

        // If this is a rotation vector, has to read all 4 tokens in a row
        if (type == 'r') {
            float u[3] = {strtof(tokens[1].c_str(), NULL), strtof(tokens[2].c_str(), NULL), strtof(tokens[3].c_str(), NULL)};
            float theta = strtof(tokens[4].c_str(), NULL);
            ts.add_rotation(u[0], u[1], u[2], theta);
        }
        // Else, if it's either a translation or a scaling vector, than read only 3 tokens
        else if (type == 't' || type == 's') {
            float v[3] = {strtof(tokens[1].c_str(), NULL), strtof(tokens[2].c_str(), NULL), strtof(tokens[3].c_str(), NULL)};
            if (type == 't') 
            { ts.add_translation(v[0], v[1], v[2]); }
            else 
            { ts.add_scaling(v[0], v[1], v[2]); }
        }
        else {
            throw "Data does not contain either a translation, a rotation or a scaling vector\n";
        }
    }
    return ts;
}

/* Create the corresponding Scene by parsing the camera, perspective and light text blocks 
 * from an opened filestream. This scene has no Objects and contains the parsed camera, perspective,
 * and light settings.
 */
Scene create_scene(ifstream &ifs, int xres, int yres) {
    string line;
    
    // Holder variables for camera parameters
    float p[3], o[3];
    float angle;

    // Holder variables for perspective parameters
    float n, f, l, r, t, b;

    // If the line is empty, we have reached the end of the camera/perspective block,
    // exit the function and return the empty scene with corresponding camera and
    // perspective setup
    while (getline(ifs, line) && !line.empty()) {
        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the info type
        string info_type = tokens[0];
        // Parses information based on the information type
        if (info_type.compare(string("position")) == 0) {
            p[0] = strtof(tokens[1].c_str(), NULL);
            p[1] = strtof(tokens[2].c_str(), NULL); 
            p[2] = strtof(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("orientation")) == 0) {
            o[0] = strtof(tokens[1].c_str(), NULL);
            o[1] = strtof(tokens[2].c_str(), NULL); 
            o[2] = strtof(tokens[3].c_str(), NULL);
            angle = strtof(tokens[4].c_str(), NULL);
        }
        else if (info_type.compare(string("near")) == 0) {
            n = strtof(tokens[1].c_str(), NULL);
        }
        else if (info_type.compare(string("far")) == 0) {
            f = strtof(tokens[1].c_str(), NULL);         
        }
        else if (info_type.compare(string("left")) == 0) {
            l = strtof(tokens[1].c_str(), NULL);       
        }
        else if (info_type.compare(string("right")) == 0) {
            r = strtof(tokens[1].c_str(), NULL); 
        }
        else if (info_type.compare(string("top")) == 0) {
            t = strtof(tokens[1].c_str(), NULL);
        }
        else if (info_type.compare(string("bottom")) == 0) {
            b = strtof(tokens[1].c_str(), NULL);      
        }
        else {
            throw "Data does not contain information relating to camera or perspective\n";
        }
    }

    // Return the vector of light sources
    vector<Light> ls = create_lights(ifs);

    // Initialize the Camera and Perspective objects
    Camera cam = Camera(p, o, angle);
    Perspective perp = Perspective(n, f, l, r, t, b);

    return Scene(cam, perp, ls, xres, yres);
}