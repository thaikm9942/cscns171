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

// Keeps track of the initial mouse positions and the ending mouse positions
int start_x, start_y, curr_x, curr_y;

// Width and height of the program
int width = 800;
int height = 800;

// Controls the keyboard interactions to zoom in and zoom out of the screen
const float step_size = 0.2;
float x_view_angle = 0, y_view_angle = 0;

// Boolean flag to keep track of whether or not a mouse button is pressed or released
bool is_pressed = false;

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;

// Vector of keyframes
vector<Frame> *all_frames;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Converts a vertex from screen coordinates to NDC coordinates
Vertex screen2ndc(float sx, float sy, int xres, int yres) {
    float ndc_x = (float) (2.0 * sx) / xres - 1.0;
    float ndc_y = (float) (yres - sy) * 2.0 / yres - 1.0;
    float squared_sum = ndc_x * ndc_x + ndc_y * ndc_y;
    if (squared_sum > 1) {
        return Vertex(ndc_x, ndc_y, 0.0);
    }
    float ndc_z = abs(sqrt(1 - (squared_sum)));
    return Vertex(ndc_x, ndc_y, ndc_z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

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
    /*
    // Add the interpolated translation, scaling and rotation here
    glTranslatef(0, 0, 0);
    glRotatef(0, 0, 0, 0);
    glScalef(0, 0, 0);
    */

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
    // Toggle next keyframe
    else if(key == 'n')
    {
        return;
    }
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

// Find the interpolated component using the Catmull-Rom splines
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
    return v * k;
}

void find_interpolated_frames(Frame k0, Frame k1, Frame k2, Frame k3) {
    return;
}

// Generate interpolated keyframes
void generate_interpolated_frames(vector<Frame> &keyframes) {
    int size = keyframes.size();
    for (int i = 0; i < size; i++) {
        Frame k_i = keyframes[i];
        Frame k_i_plus_one = keyframes[(i + 1) % size];
        Frame k_i_minus_one = keyframes[((i - 1) + size) % size];
        Frame k_i_plus_two = keyframes[(i + 2) % size];
        find_interpolated_frames(k_i_minus_one, k_i, k_i_plus_one, k_i_plus_two);
    }
}

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
            vector<Frame> frames;

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
                        angle = deg2rad(atof(tokens[idx1].c_str()));
                        frames.push_back(Frame(frame_id, t, s, r, angle, true));
                    }
                }
            }

            for (int i = 0; i < frames.size(); i++) {
                frames[i].print_frame();
            }

            // Generate interpolated frames between every 2 keyframes
            // generate_interpolated_frames(frames);

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
