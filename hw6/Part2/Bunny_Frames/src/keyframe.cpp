// Includes for OpenGL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Includes for Eigen library and parser.h and halfedge.h
#include "../Eigen/Dense"
#include "../include/vertex.h"
#include "../include/object.h"
#include "../include/frame.h"
#include "../include/utils.h"
#include "../include/halfedge.h"

// Includes for standard C library
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <string>
#define _USE_MATH_DEFINES

// C++ data structures
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////

/* Defining prototype functions we want to write for OpenGL */

void init(void);
void init_lights();
void set_lights();
void reshape(int width, int height);
void display(void);
void key_pressed(unsigned char key, int x, int y);

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////
///      GLOBAL VARIABLES         ///
/////////////////////////////////////
// Keeps track of the initial mouse positions and the ending mouse positions
int start_x, start_y, curr_x, curr_y;

// Width and height of the program
int width = 800;
int height = 800;

// Vector of keyframes
vector<Frame> all_frames;

// Current frame id to load the corresponding frame
int curr_frame_id;

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////
///      OPENGL FUNCTIONS       ///
///////////////////////////////////

/*
 * Function definitions for prototype functions defined above. These functions are passed 
 * into OpenGL functions to handle displaying of graphics.
 */

// Initialization function
void init(void) {
    // Enables Gouraud Shading
    glShadeModel(GL_SMOOTH);

    // Enables backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enables depth buffering
    glEnable(GL_DEPTH_TEST);

    // Enables auto-normalization of vectors
    glEnable(GL_NORMALIZE);

    // Set the matrix modified to be the projection matrix
    glMatrixMode(GL_PROJECTION);

    // Load the identity matrix as the projection matrix
    glLoadIdentity();

    // Enables vertex buffer array and normal buffer array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Create the perspective projection matrix using frustum parameters - these are hardcoded for the purpose
    // of the assignment (taken from the scene file for the bunny from HW 5)
    glFrustum(-0.5, 0.5, 
        -0.5, 0.5, 
        1.0, 10.0);

    // Set the matrix modified to be the main matrix
    glMatrixMode(GL_MODELVIEW);

    // Initialize lights
    init_lights();
}

// Initialize the light sources in the scene
void init_lights() {
    // Enables lighting calculations in OpenGL
    glEnable(GL_LIGHTING);
    
    // Create light source 1
    glEnable(GL_LIGHT0);

    // Hardcode color values and attenuation constant
    float c0[3] = {0.0, 0.6, 0.8};
    float k0 = 0.0;

    // Set up ambient, diffuse, specular light color and attenuation for light 1
    glLightfv(GL_LIGHT0, GL_AMBIENT, c0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, c0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, c0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, k0);

    // Create light source 2
    glEnable(GL_LIGHT1);

    // Hardcode color values and attenuation constant
    float c1[3] = {0.0, 0.1, 0.2};
    float k1 = 0.0;

    // Set up ambient, diffuse, specular light color and attenuation for light 2
    glLightfv(GL_LIGHT1, GL_AMBIENT, c1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, c1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, c1);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, k1);
}

// Sets the light sources position in the scene
void set_lights() {
    // Hardcode the light source positions - refer to scene_bunny.txt from HW 5
    float p0[4] = {5.0, 5.0, 5.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, p0);

    float p1[4] = {-5.0, 5.0, -5.0, 1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, p1);
}

// Reshape function
void reshape(int width, int height) {
    // Sets the minimum screen size to be 1 x 1
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;

    // Sets up Viewport to convert NDC to screen coordinates
    glViewport(0, 0, width, height);

    // Re-renders the scene after resizing
    glutPostRedisplay();
}

// Draw the frame with the given bunny Object
void draw_bunny(Object* bunny) {
    // Push a copy of the current Modelview Matrix onto the Stack
    glPushMatrix();

    // Hardcoded geometric transformations for the bunny object
    glScalef(1, 1, 1);
    glRotatef(rad2deg(0.0), 1.0, 0.0, 0.0);
    glTranslatef(0, 0, 0);
    
    // Set the material properties of the object being rendered
    float amb[3] = {0.5, 0.5, 0.5};
    float dif[3] = {0.5, 0.5, 0.5};
    float spec[3] = {0.5, 0.5, 0.5};
    float shininess = 0.1;

    // Set material properties for OpenGL
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // Set the pointer to the vertex buffer array of the object being rendered
    glVertexPointer(3, GL_FLOAT, 0, &bunny->vertex_buffer[0]);

    // Set the pointer to the normal buffer array of the object being rendered
    glNormalPointer(GL_FLOAT, 0, &bunny->normal_buffer[0]);

    int buffer_size = bunny->vertex_buffer.size();
    glDrawArrays(GL_TRIANGLES, 0, buffer_size);

    // Retrieve the Modelview Matrix pretransformation of an object
    glPopMatrix();
}

// Function to draw the text representing the current frame number on the top right corner of the window
void drawText() {
    // String buffer
    unsigned char buffer[100];
    if (all_frames[curr_frame_id].is_keyframe) {
        strcpy((char*) buffer, "KEYFRAME: ");
    }
    else {
        strcpy((char*) buffer, "FRAME: ");
    }  

    // Convert the current frame id to text
    stringstream ss;
    ss << curr_frame_id;
    string id = ss.str();

    // Add the frame id to the string buffer
    strcat((char*) buffer, (const char*) id.c_str());

    // Draw text
    int w;
    w = glutBitmapLength(GLUT_BITMAP_8_BY_13, buffer);

    // Set position of text
    glWindowPos2f(width - 100, height - 50);

    // Set color of text
    glColor3f(1., 1., 1.);
    int len = strlen((const char*) buffer);

    // Draw each character in the frame text
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
    }
}

// Main "display" function. Displays the current object of the current frame (which is retrieved by a global
// frame id variable)
void display(void) {
    // Reset color buffer and depth buffer array
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the Modelview matrix to be the Identity matrix
    glLoadIdentity();

    // Remember to apply inverse camera transforms BEFORE the actual
    // geometric transforms due to OpenGL Post-Multiplication behavior

    // Camera is hardcoded here using the values from the scene_bunny.txt file form HW 5
    // Apply inverse camera transform, including rotation AND then translation
    glRotatef(-rad2deg(0.0), 0.0, -1.0, 0.0);
    glTranslatef(0, 0, -3);

    // Set the light sources positions
    set_lights();

    // Only draw bunnies if there are more than 0 frames
    if (all_frames.size() > 0) {
        // Draw the bunny object in the current frame
        draw_bunny(all_frames[curr_frame_id].object);
    }

    // Display the text showing what frame we are in
    drawText();

    // Swap the active and off-screen buffers with one another
    glutSwapBuffers();
}

// Handle key events when a key is pressed
void key_pressed(unsigned char key, int x, int y) {
    // Quitting the program
    if(key == 'q')
    {
        exit(0);
    }
    // Toggle next keyframe and loop back as needed
    else if(key == 'n')
    {
        // If there are no frames, then do nothing
        if (all_frames.size() == 0) {
            return;
        }
        curr_frame_id = (curr_frame_id + 1) % all_frames.size();
        glutPostRedisplay();
    }
}

//////////////////////////////////////////////
///    KEYFRAME INTERPOLATION FUNCTIONS    ///
//////////////////////////////////////////////

// Find the interpolated component using the Catmull-Rom spline (here, the B matrix corresponds to
// tension parameter t = 0.0)
float find_interpolated_value(Eigen::Vector4f p, float u) {
    // Construct our inverse matrix C^-1 = B - 4 x 4 matrix
    Eigen::Matrix4f B;
    B << 0, 2, 0, 0,
        -1, 0, 1, 0,
        2, -5, 4, -1,
        -1, 3, -3, 1;
    B = B / 2;

    // 1 x 4 row vector
    Eigen::RowVector4f v;
    v << 1, u, u * u, u * u * u;

    // Compute the product of B * p
    Eigen::Vector4f k = B * p;

    // Return the final interpolated component
    return v * k;
}

// This function calculates the interpolated vertex given four control vertices and the step paramter
Vertex find_interpolated_vertex(Vertex v0, Vertex v1, Vertex v2, Vertex v3, float u) {
    float x = find_interpolated_value(Eigen::Vector4f(v0.x, v1.x, v2.x, v3.x), u);
    float y = find_interpolated_value(Eigen::Vector4f(v0.y, v1.y, v2.y, v3.y), u);
    float z = find_interpolated_value(Eigen::Vector4f(v0.z, v1.z, v2.z, v3.z), u);
    return Vertex(x, y, z);
}

// This function returns a Frame containing an object whose vector of vertices were interpolated
// using the four control frames' object's vertices and the step paramter u
Frame find_interpolated_frame(Frame k0, Frame k1, Frame k2, Frame k3, float u, int id) {
    Object* interpolated_object = new Object();
    for (int i = 0; i < k0.object->vertices.size(); i++) {
        Vertex interpolated_vertex = find_interpolated_vertex(k0.object->vertices[i], k1.object->vertices[i],
            k2.object->vertices[i], k3.object->vertices[i], u);
        interpolated_object->vertices.push_back(interpolated_vertex);
    }

    // The faces are the same across all frame objects, so we just the faces of interpolated object
    // to be the face of the object of any control frame
    interpolated_object->faces = k0.object->faces;
    return Frame(id, interpolated_object, false);
}

// Generate all interpolated frames between every pair of keyframes
void generate_interpolated_frames(vector<Frame> &keyframes) {
    int size = keyframes.size();
    for (int i = 0; i < size - 1; i++) {
        // Get keyframes i and i + 1
        Frame k1 = keyframes[i];
        Frame k2 = keyframes[i + 1];

        // Get keyframes i - 1 and i + 2. If this is the first keyframe, then the first and second control frame is 
        // both keyframe 0. If this is the second to last keyframe, then the third and fourth control frame is both
        // the last keyframe.
        Frame k0 = keyframes[max(0, i - 1)];
        Frame k3 = keyframes[min(size - 1, i + 2)];

        // Calculate the number of interpolated frames in between k_i and k_{i+1} (including both keyframes)
        int num_interpolated_frames = k2.frame_id - k1.frame_id;

        // Calculate the step size for the u parameter
        float step_size = 1.0 / num_interpolated_frames;

        // Calculate the interpolated frame ID (this is added to the starting keyframe ID to obtain the final ID)
        int interpolated_id = 1;
        
        // Add the first keyframe into the vector of all frames
        all_frames.push_back(k1);

        // Add all interpolated frames in between the two keyframes into the vector of all frames
        for (float u = step_size; u < 1.0; u += step_size) {
            Frame interpolated_frame = find_interpolated_frame(k0, k1, k2, k3, u, k1.frame_id + interpolated_id);
            all_frames.push_back(interpolated_frame);
            interpolated_id++;
        }
    }

    // Add the last keyframe to our vector of all frames
    all_frames.push_back(keyframes[size - 1]);
}

//////////////////////////////
///    PARSING FUNCTIONS   ///
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

///////////////////////////////
///    HALFEDGE FUNCTIONS   ///
///////////////////////////////

// Function to fill in the vertex and normal buffers for each object in a  frame
void fill_buffers() {
    for (int i = 0; i < all_frames.size(); i++) {
        // Retrieves the current frame and its corresponding object
        Frame curr_frame = all_frames[i];
        Object* object = curr_frame.object;

        // Retrieves the mesh data for this object
        Mesh_Data* mesh_data = create_mesh_data(*object);

        // Allocate new pointers to the store halfedge vertices and halfedge faces
        vector<HEV*> *hevs = new vector<HEV*>;
        vector<HEF*> *hefs = new vector<HEF*>;

        // Build the halfedge data structure representation of this object
        build_HE(mesh_data, hevs, hefs);

        // Compute the vertex normals for each vertex in the halfedge data structure,
        // need to ignore the first vertex since that's NULL
        for (int j = 1; j < hevs->size(); j++) {
            compute_vertex_normal(hevs->at(j));
        }

        // For each halfedge face, we will add the 3 vertices that form the face and their
        // respective vertex normals into the buffers
        for (int j = 0; j < hefs->size(); j++) {
            // Obtain the face at index j
            HEF* hef = hefs->at(j);

            // Get the corresponding halfedge of this face
            HE* he = hef->edge;

            // Retrieves the 3 vertices that form this face
            HEV* v1 = he->vertex;
            HEV* v2 = he->next->vertex;
            HEV* v3 = he->next->next->vertex;
            
            // Add the vertices into the vertex buffer
            object->add_vertex_to_buffer(Vertex(v1->x, v1->y, v1->z));
            object->add_vertex_to_buffer(Vertex(v2->x, v2->y, v2->z));
            object->add_vertex_to_buffer(Vertex(v3->x, v3->y, v3->z));

            // Add their corresponding vertex normals into the normal buffer
            object->add_normal_to_buffer(v1->normal);
            object->add_normal_to_buffer(v2->normal);
            object->add_normal_to_buffer(v3->normal);
        }

        // Delete the mesh data used to calculate our normals
        delete_HE(hevs, hefs);
    }
}

//////////////////////////////
///      MAIN FUNCTION     ///
//////////////////////////////

// Main function where the parsing is done and everything comes together
int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("Usage: ./keyframe [keyframe1.obj] [keyframe2.obj] [keyframe3.obj] [keyframe4.obj] [keyframe5.obj]\n");
    }
    else {
        // Clear all objects
        all_frames.clear();

        // Vector to store all the keyframes of the bunny object
        vector<Frame> keyframes;
        
        // Process all keyframe object
        for (int i = 1; i < 6; i++) {
            // Open the bunny object file and create its corresponding object from it
            Object bunny_object = create_object(argv[i]); 
            Frame bunny_frame = Frame((i - 1) * 5, new Object(bunny_object), true);
            keyframes.push_back(bunny_frame);
        }

        // Generate all interpolated frames between every pair of keyframes
        generate_interpolated_frames(keyframes);

        // Fill the vertex and vertex normal buffers of each frame object
        fill_buffers();

        // Set the current frame id to 0
        curr_frame_id = 0;
        
        // After preprocessing, we now render the scene
        glutInit(&argc, argv);

        // Initialize OpenGL display with double, RGB and depth buffers
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

        // Initialize window size
        glutInitWindowSize(width, height);

        // Set top-left corner of window to be (0, 0)
        glutInitWindowPosition(0, 0);

        // Create window with name "Shader"
        glutCreateWindow("Shader");

        // Call our init function
        init();

        // Set OpenGL display function to our display function
        glutDisplayFunc(display);

        // Set OpenGL reshape function to our reshape function
        glutReshapeFunc(reshape);

        // Set OpenGL keyboard handler to be our key_pressed function
        glutKeyboardFunc(key_pressed);

        // Run our event processing loop
        glutMainLoop();
    }
}
