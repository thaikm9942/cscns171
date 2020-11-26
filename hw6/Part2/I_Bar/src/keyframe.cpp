// Includes for OpenGL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Includes for Eigen library and parser.h and quaternion.h and halfedge.h
#include "../Eigen/Dense"
#include "../include/quaternion.h"
#include "../include/vertex.h"
#include "../include/frame.h"
#include "../include/utils.h"

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

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;

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

    // Initialize quadratic
    quadratic = gluNewQuadric();

    // Set the matrix modified to be the projection matrix
    glMatrixMode(GL_PROJECTION);

    // Load the identity matrix as the projection matrix
    glLoadIdentity();

    // Create the perspective projection matrix using frustum parameters - these are hardcoded for the purpose
    // of the assignment
    glFrustum(-1.0, 1.0, 
        -1.0, 1.0, 
        1.0, 60.0);

    // Set the matrix modified to be the main matrix
    glMatrixMode(GL_MODELVIEW);
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


void drawIBar()
{
    // If there are no frames, then do nothing
    if (all_frames.size() == 0) {
        return;
    }
    
    // Add the interpolated translation, scaling and rotation geometric transformation based on the current frame
    Frame frame = all_frames[curr_frame_id];
    glTranslatef(frame.t[0], frame.t[1], frame.t[2]);
    glRotatef(frame.angle, frame.r[0], frame.r[1], frame.r[2]);
    glScalef(frame.s[0], frame.s[1], frame.s[2]);

    /* Parameters for drawing the cylinders */
    float cyRad = 0.2, cyHeight = 1.0;
    int quadStacks = 4, quadSlices = 4;
    
    glPushMatrix();
    glColor3f(0, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
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
    glColor3f(1., 0., 0.);
    int len = strlen((const char*) buffer);

    // Draw each character in the frame text
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
    }
}

// Main "display" function
void display(void) {
    // Reset color buffer and depth buffer array
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the Modelview matrix to be the Identity matrix
    glLoadIdentity();

    // Remember to apply inverse camera transforms BEFORE the actual
    // geometric transforms due to OpenGL Post-Multiplication behavior

    // Camera is hardcoded here at position (0, 0, 40) for the purpose of the assignment
    // Apply inverse camera transform, including rotation AND then translation
    glTranslatef(0, 0, -40);

    // Draw I-Bar
    drawIBar();

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

// Compute the interpolated translation vector
void find_interpolated_translation(float t[3], Frame k0, Frame k1, Frame k2, Frame k3, float u) {
    t[0] = find_interpolated_value(Eigen::Vector4f(k0.t[0], k1.t[0], k2.t[0], k3.t[0]), u);
    t[1] = find_interpolated_value(Eigen::Vector4f(k0.t[1], k1.t[1], k2.t[1], k3.t[1]), u);
    t[2] = find_interpolated_value(Eigen::Vector4f(k0.t[2], k1.t[2], k2.t[2], k3.t[2]), u);
}

// Compute the interpolated rotation vector
void find_interpolated_scaling(float s[3], Frame k0, Frame k1, Frame k2, Frame k3, float u) {
    s[0] = find_interpolated_value(Eigen::Vector4f(k0.s[0], k1.s[0], k2.s[0], k3.s[0]), u);
    s[1] = find_interpolated_value(Eigen::Vector4f(k0.s[1], k1.s[1], k2.s[1], k3.s[1]), u);
    s[2] = find_interpolated_value(Eigen::Vector4f(k0.s[2], k1.s[2], k2.s[2], k3.s[2]), u);
}

// Compute the interpolated quaternion component-wise
Quaternion find_interpolated_quaternion(Frame k0, Frame k1, Frame k2, Frame k3, float u) {
    float s = find_interpolated_value(Eigen::Vector4f(k0.q.s, k1.q.s, k2.q.s, k3.q.s), u);
    float x = find_interpolated_value(Eigen::Vector4f(k0.q.v[0], k1.q.v[0], k2.q.v[0], k3.q.v[0]), u);
    float y = find_interpolated_value(Eigen::Vector4f(k0.q.v[1], k1.q.v[1], k2.q.v[1], k3.q.v[1]), u);
    float z = find_interpolated_value(Eigen::Vector4f(k0.q.v[2], k1.q.v[2], k2.q.v[2], k3.q.v[2]), u);
    return Quaternion(s, x, y, z);
}

// Compute the interpolated rotation vector given its quaternion representatino
void find_interpolated_rotation(float r[3], Quaternion new_q) {
    // If the real component (s) is 1, then our square root is 0, 
    // then we can set rotation axis to be any arbitrary axis since the angle of rotation is 0
    if (sqrt(1 - new_q.s * new_q.s) < 10e-6) {
        r[0] = 0;
        r[1] = 0;
        r[2] = 1;
    }
    else {
        r[0] = new_q.v[0] / sqrt(1 - new_q.s * new_q.s);
        r[1] = new_q.v[1] / sqrt(1 - new_q.s * new_q.s);
        r[2] = new_q.v[2] / sqrt(1 - new_q.s * new_q.s);
    }
}

Frame find_interpolated_frame(Frame k0, Frame k1, Frame k2, Frame k3, float u, int id) {
    // Calculate the interpolated translation vector
    float t[3];
    find_interpolated_translation(t, k0, k1, k2, k3, u);

    // Calculate the interpolated scaling vector
    float s[3];
    find_interpolated_scaling(s, k0, k1, k2, k3, u);

    // Calculate the interpolated quaternion
    Quaternion q = find_interpolated_quaternion(k0, k1, k2, k3, u);

    // Normalize the quaternion after interpolation
    q.normalize();

    // Calculates the new rotation axis
    float r[3];
    find_interpolated_rotation(r, q);

    // Calculates the new rotation angle
    float angle = rad2deg(2 * acos(q.s));

    // Returns the interpolated frame corresponding to the step iteration u
    return Frame(id, t, s, r, angle, q, false);
}

// Generate all interpolated frames between every pair of keyframes
void generate_interpolated_frames(vector<Frame> &keyframes) {
    int size = keyframes.size();
    for (int i = 0; i < size - 1; i++) {
        // Get keyframes i and i + 1
        Frame k1 = keyframes[i];
        Frame k2 = keyframes[i + 1];

        // Get keyframes i - 1 and i + 2
        Frame k0 = keyframes[((i - 1) + size) % size];
        Frame k3 = keyframes[(i + 2) % size];

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
}

//////////////////////////////
///    PARSING FUNCTIONS   ///
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

//////////////////////////////
///      MAIN FUNCTION     ///
//////////////////////////////

// Main function where the parsing is done and everything comes together
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./keyframe [test_script.script]\n");
    }
    else {
        // Initialize filestream
        ifstream ifs;

        // Open the script file
        ifs.open(argv[1], ifstream::in);

        // If file opening is unsuccessful, print out an error message
        if (!ifs.is_open()) {
            cout << "Error opening file\n";
        }
        else { 
            // Initialize variable to store the number of frames and the frame objects themselves
            int num_frames;
            vector<Frame> keyframes;

            // Line to be read
            string line;

            // Variables to hold the values for the parsed translation, scaling and rotation components
            int frame_id;   
            float t[3];
            float s[3];
            float r[3];
            float angle;   

            // Parsing text and loading all data into their appropriate data structures
            while (getline(ifs, line)) {
                vector<string> tokens = strsplit(line, ' ');

                // If there's only 1 token, this is the frame number
                if (tokens.size() == 1) {
                    num_frames = atoi(tokens[0].c_str());
                }
                // If there's 2 tokens, this is the line containing the keyframe numeber
                else if (tokens.size() == 2) {
                    frame_id = atoi(tokens[1].c_str());
                }
                // If there are more than 4 tokens, then these are the translation, scaling and rotation vectors
                else if (tokens.size() >= 4) {
                    // Gets the type of transformation vector
                    string type = tokens[0];   

                    // Weird indexing since strsplit only splits by a single white space character 
                    int idx1 = tokens.size() - 1;     
                    int idx2 = tokens.size() - 2; 
                    int idx3 = tokens.size() - 3; 
                    int idx4 = tokens.size() - 4;          
                    if (type.compare(string("translation")) == 0) {
                        t[0] = atof(tokens[idx3].c_str());
                        t[1] = atof(tokens[idx2].c_str());
                        t[2] = atof(tokens[idx1].c_str());
                    }
                    else if (type.compare(string("scale")) == 0) {
                        s[0] = atof(tokens[idx3].c_str());
                        s[1] = atof(tokens[idx2].c_str());
                        s[2] = atof(tokens[idx1].c_str());
                    }
                    else if (type.compare(string("rotation")) == 0) {
                        r[0] = atof(tokens[idx4].c_str());
                        r[1] = atof(tokens[idx3].c_str());
                        r[2] = atof(tokens[idx2].c_str());
                        angle = atof(tokens[idx1].c_str());
                        keyframes.push_back(Frame(frame_id, t, s, r, angle, true));
                    }
                }
            }

            // Last keyframe is just the first frame again
            Frame last_frame = keyframes[0];
            last_frame.frame_id = num_frames;
            keyframes.push_back(last_frame);

            // Re-initialize all frames in the scene
            all_frames = vector<Frame>();

            // Generate all interpolated frames between every all keyframes
            generate_interpolated_frames(keyframes);

            // Set the current frame id to 0
            curr_frame_id = 0;

            // After the scene parsing business is done, render the scene
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
}
