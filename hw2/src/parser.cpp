#include <string.h>
#include "../include/parser.h"

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
        // strings as vertex double coordinates 
        if (type.compare(string("v")) == 0) {
            Vertex v = Vertex(strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL));
            obj.add(v);
        }
        // If the first character of the line is 'vn', the process the
        // strings as vertex normal double coordinates
        else if (type.compare(string("vn")) == 0) {
            Vertex vn = Vertex(strtod(n1.c_str(), NULL), strtod(n2.c_str(), NULL), strtod(n3.c_str(), NULL));
            obj.add_normal(vn);
        }
        // If the first character is 'f' instead, then process the strings
        // as the vertex and vertex normal integer indices
        else if (type.compare(string("f")) == 0) {
            int i[3];
            int n[3];

            // Indices of vertex
            i[0] = (int) n1[0] - '0';
            i[1] = (int) n2[0] - '0';
            i[2] = (int) n3[0] - '0';

            // Indices of vertex normal
            n[0] = (int) n1[n1.length() - 1] - '0';
            n[1] = (int) n2[n2.length() - 1] - '0';
            n[2] = (int) n3[n3.length() - 1] - '0';

            // Index of vertex normal
            Face f = Face(i, n);
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
        double l[3], c[3];
        double k;

        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the info type
        string info_type = tokens[0];

        if (info_type.compare(string("light")) == 0) {
            l[0] = strtod(tokens[1].c_str(), NULL);
            l[1] = strtod(tokens[2].c_str(), NULL); 
            l[2] = strtod(tokens[3].c_str(), NULL);

            // Skips 1 to account for commas
            c[0] = strtod(tokens[5].c_str(), NULL);
            c[1] = strtod(tokens[6].c_str(), NULL); 
            c[2] = strtod(tokens[7].c_str(), NULL);

            // Skips 1 again to account for commas
            k = strtod(tokens[9].c_str(), NULL);
            
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
    double a[3], d[3], s[3];
    double p;

    while (getline(ifs, line) && !line.empty()) {
        // We parse the tokens to identify
        vector<string> tokens = strsplit(line, ' ');
        
        // Parse the info type
        string info_type = tokens[0];

        // Parses information based on the information type
        if (info_type.compare(string("ambient")) == 0) {
            a[0] = strtod(tokens[1].c_str(), NULL);
            a[1] = strtod(tokens[2].c_str(), NULL); 
            a[2] = strtod(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("diffuse")) == 0) {
            d[0] = strtod(tokens[1].c_str(), NULL);
            d[1] = strtod(tokens[2].c_str(), NULL); 
            d[2] = strtod(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("specular")) == 0) {
            s[0] = strtod(tokens[1].c_str(), NULL);
            s[1] = strtod(tokens[2].c_str(), NULL); 
            s[2] = strtod(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("shininess")) == 0) {
            p = strtod(tokens[1].c_str(), NULL);
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

/* Create the corresponding Scene by parsing the camera, perspective and light text blocks 
 * from an opened filestream. This scene has no Objects and contains the parsed camera, perspective,
 * and light settings.
 */
Scene create_scene(ifstream &ifs) {
    string line;
    
    // Holder variables for camera parameters
    double p[3], o[3];
    double angle;

    // Holder variables for perspective parameters
    double n, f, l, r, t, b;

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
            p[0] = strtod(tokens[1].c_str(), NULL);
            p[1] = strtod(tokens[2].c_str(), NULL); 
            p[2] = strtod(tokens[3].c_str(), NULL);
        }
        else if (info_type.compare(string("orientation")) == 0) {
            o[0] = strtod(tokens[1].c_str(), NULL);
            o[1] = strtod(tokens[2].c_str(), NULL); 
            o[2] = strtod(tokens[3].c_str(), NULL);
            angle = strtod(tokens[4].c_str(), NULL);
        }
        else if (info_type.compare(string("near")) == 0) {
            n = strtod(tokens[1].c_str(), NULL);
        }
        else if (info_type.compare(string("far")) == 0) {
            f = strtod(tokens[1].c_str(), NULL);         
        }
        else if (info_type.compare(string("left")) == 0) {
            l = strtod(tokens[1].c_str(), NULL);       
        }
        else if (info_type.compare(string("right")) == 0) {
            r = strtod(tokens[1].c_str(), NULL); 
        }
        else if (info_type.compare(string("top")) == 0) {
            t = strtod(tokens[1].c_str(), NULL);
        }
        else if (info_type.compare(string("bottom")) == 0) {
            b = strtod(tokens[1].c_str(), NULL);      
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

    return Scene(cam, perp, ls);
}