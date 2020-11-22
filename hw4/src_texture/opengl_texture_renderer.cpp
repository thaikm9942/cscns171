// Includes for OpenGL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Includes for Eigen library and parser.h and quaternion.h
#include "../Eigen/Dense"
#include "../include/parser.h"
#include "../include/quaternion.h"

// Includes for standard c library
#include <math.h>
#include <cstdlib>
#include <string>
#define _USE_MATH_DEFINES

// C++ data structures
#include <vector>
#include <map>

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////

// Reading the PNG texture files in
extern GLenum readpng(const char *filename);

/* Defining prototype functions we want to write for OpenGL */

void init(void);
void reshape(int width, int height);
void display(void);

void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Keep tracks of the width and height of the screen
int xres, yres;

// Keeps track of the initial mouse positions and the ending mouse positions
int start_x, start_y, curr_x, curr_y;

// Keeps track of the last arcball rotation and the current arcball rotation
Quaternion last_rotation, curr_rotation;

// Boolean flag to keep track of whether or not a mouse button is pressed or released
bool is_pressed = false;

// Name of the shader program and the vertex shader program filename and the fragment
// shader program filename
static GLenum shader;
static string vert_filename, frag_filename;

// Name of the texture map and normal map to be loaded
static GLenum texture_map, normal_map;
static GLint texture_uniform_pos, normal_uniform_pos;

///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

/*
 * Useful helper functions for general calculation purposes
 */

void normalize(float u[3]) {
    float norm = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
    u[0] = u[0] / norm;
    u[1] = u[1] / norm;
    u[2] = u[2] / norm;
}

// Helper function to convert an angle from degree to radian
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

// Helper function to convert an angle from radians to degree
float rad2deg(float angle)
{
    return angle * 180.0 / M_PI;
}

// Calculate the Euclidean norm of a vector
float norm(Vertex v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Calculate the dot product between 2 vectors
float dot(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Calculate the cross product between 2 vectors 
Vertex cross(Vertex v1, Vertex v2) {
    double cx = v1.y * v2.z - v1.z * v2.y;
    double cy = v1.z * v2.x - v1.x * v2.z;
    double cz = v1.x * v2.y - v1.y * v2.x;
    return Vertex(cx, cy, cz);
}

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

// Get the current Arcball Quaternion representing the current rotation matrix
Quaternion get_current_rotation() {
    return curr_rotation.dot(last_rotation);
}

// Converts a Quaternion to a rotation matrix. The details are outlined in Lecture Notes for Assignment 3
Eigen::Matrix4f quar2rot(Quaternion q) {
    float qs = q.s;
    float qx = q.v[0];
    float qy = q.v[1];
    float qz = q.v[2];
    // Constructing the first row values
    float r11 = 1 - 2 * qy * qy - 2 * qz * qz;
    float r12 = 2 * (qx * qy - qz * qs);
    float r13 = 2 * (qx * qz + qy * qs);

    // Constructing the second row values
    float r21 = 2 * (qx * qy + qz * qs);
    float r22 = 1 - 2 * qx * qx - 2 * qz * qz;
    float r23 = 2 * (qy * qz - qx * qs);

    // Constructing the third row values
    float r31 = 2 * (qx * qz - qy * qs);
    float r32 = 2 * (qy * qz + qx * qs);
    float r33 = 1 - 2 * qx * qx - 2 * qy * qy;

    Eigen::Matrix4f r;
    r << r11, r12, r13, 0, // row 1
        r21, r22, r23, 0, // row 2
        r31, r32, r33, 0, // row 3
        0, 0, 0, 1; // row 4

    // Asserts that the product of a rotation matrix and its transpose 
    // should be the identity matrix
    assert((r * r.transpose()).isApprox(Eigen::Matrix4f::Identity(), 10e-7));
    return r;
}

// Compute the rotation matrix for the Arcball rotation given the starting (x, y) coordinates
// and the final (x, y) coordinates of the mouse
Quaternion compute_rotation_Quaternion(float curr_x, float curr_y, float start_x, float start_y) {
    // Convert both starting coordinates and ending coordinates from screen to NDC
    Vertex curr_ndc = screen2ndc(curr_x, curr_y, xres, yres);
    Vertex start_ndc = screen2ndc(start_x, start_y, xres, yres);

    // Calculate the axis of rotation using cross product
    Vertex axis = cross(start_ndc, curr_ndc);
    float u[3] = {axis.x, axis.y, axis.z};

    // Compute the angle of rotation
    float arg = dot(start_ndc, curr_ndc) / (norm(start_ndc) * norm(curr_ndc));
    float theta = acosf(fmin(1.0, arg));

    // Converts the rotation matrix parameters to a Quaternion
    return rot2quar(u, theta);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function definitions for prototype functions defined above. These functions are passed 
 * into OpenGL functions to handle displaying of graphics.
 */

// Initialization function
void init(void) {
    // Enables depth buffering
    glEnable(GL_DEPTH_TEST);

    // Enables auto-normalization of vectors
    glEnable(GL_NORMALIZE);

    // Enables vertex buffer array and normal buffer array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    // Enables the texture coordinate buffers
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Initialize the Arcball rotation matrices as identity Quaternions
    last_rotation = Quaternion_identity();
    curr_rotation = Quaternion_identity();

    // Initialize lighting model. This is the same lighting source as the one in the shader demo
    GLfloat pos[] = {7.0, 2.0, 3.0, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    // Initialize the material properties. This is same material as the one in the shader demo
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    GLfloat black[4] = {0,0,0,0};

    // Set the default ambient and specular color values to be black
    glMaterialfv(GL_FRONT, GL_AMBIENT, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);

    // Set the matrix modified to be the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Recalculate the aspect ratio of the screen and reinitialize the projection matrix
    double aspect = xres / (double) yres;
    // Load the perspective projection matrix using the aspect ratio of the screen
    gluPerspective(60, aspect, 1, 50);
}

// Reshape function
void reshape(int width, int height) {
    // Sets the minimum screen size to be 1 x 1
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;

    // Sets up Viewport to convert NDC to screen coordinates
    glViewport(0, 0, width, height);

    // Set the matrix modified to be the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Recalculate the aspect ratio of the screen and reinitialize the projection matrix
    double aspect = width / (double) height;
    // Load the perspective projection matrix using the aspect ratio of the screen
    gluPerspective(60, aspect, 1, 50);

    // Re-renders the scene after resizing
    glutPostRedisplay();
}

// Main "display" function
void display(void) {
    // Reset color buffer and depth buffer array
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW_MATRIX);
    // Set the Modelview matrix to be the Identity matrix

    glLoadIdentity();
    // Remember to apply inverse camera transforms BEFORE the actual
    // geometric transforms due to OpenGL Post-Multiplication behavior

    // Apply inverse camera transform
    glTranslatef(0, 0, 0);

    // Converts the Quaternion to rotation matrix and apply this Arcball rotation to all points
    Quaternion arcball_quar = get_current_rotation();

    // Normalize the Quaternion to be a unit Quaternion
    arcball_quar.normalize();
    Eigen::Matrix4f arcball_rotation = quar2rot(arcball_quar);
    glMultMatrixf(arcball_rotation.data());

    // Draw objects
    draw_objects();

    // Swap the active and off-screen buffers with one another
    glutSwapBuffers();
}

// Load our shader programs
void read_shaders() {
    /* Reading in source code files */

    // Specifies the strings that serves as the vertical shader and the fragment
    // shader program source code (in this case, the string is basically the
    // contents of the whole file)
   string vert_program_source, frag_program_source;
   
   // Opens the vertex shader program file
   ifstream vert_file(vert_filename.c_str());
   if (!vert_file) {
       cerr << "Error opening vertex shader program\n";
       exit(0);
   }

   // Opens the fragment shader program file
   ifstream frag_file(frag_filename.c_str());
   if (!frag_file) {
       cerr << "Error opening fragment shader program\n";
       exit(0);
   }
   // Get the contents/source code from the file into the program source strings
   getline(vert_file, vert_program_source, '\0');

   // This is the array of strings of source code for the vertex shader program
   const char* vert_shader_source = vert_program_source.c_str();

   // Similar to above, but is done with the fragment shader program instead
   getline(frag_file, frag_program_source, '\0');
   const char* frag_shader_source = frag_program_source.c_str();

   /* Creating our vertex shader */

   // Initialize our vertex shader
   GLenum vert_shader;

   // Initialize general shader program
   shader = glCreateProgram();

   // Create vertex shader
   vert_shader = glCreateShader(GL_VERTEX_SHADER);

   // glShaderSource takes in the shader object to whose source code is
   // to be replaced and takes in the pointer to our source code. The fourth
   // argument is an array of string lengths, which we specify to be NULL
   // because there's only a single string of source code
   glShaderSource(vert_shader, 1, &vert_shader_source, NULL);

   // Compiles the vertex shader
   glCompileShader(vert_shader);
   
   // Flag to check if compilation is complete
   GLint is_compiled = 0;

   // Get the final shader after compiling the code. The 2nd argument specifies
   // the compilation status of the shader, and writes it to the 3rd argument,
   // which is the flag variable above.
   glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &is_compiled);

   // If compilation fails, output error information log
   if(is_compiled == GL_FALSE)
   {
      GLint max_length = 0;
      glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &max_length);
    
      // The max_length includes the NULL character
      vector<GLchar> error_log(max_length);
      glGetShaderInfoLog(vert_shader, max_length, &max_length, &error_log[0]);
    
      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      for (int i = 0; i < error_log.size(); i++)
         cout << error_log[i];
      glDeleteShader(vert_shader); // Don't leak the shader.
      return;
   }

   cerr << "Making fragment shader\n";
   /* Creating our fragment shader */

   // Initialize our fragment shader
   GLenum frag_shader;

   // Similar to the above, but with fragment shader instead
   frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
   glCompileShader(frag_shader);

   is_compiled = 0;
   glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &is_compiled);

   // If compilation fails, output error information log
   if(is_compiled == GL_FALSE)
   {
      GLint max_length = 0;
      glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &max_length);
    
      // The max_length includes the NULL character
      std::vector<GLchar> error_log(max_length);
      glGetShaderInfoLog(frag_shader, max_length, &max_length, &error_log[0]);
    
      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      for (int i = 0; i < error_log.size(); i++)
         cout << error_log[i];
      glDeleteShader(frag_shader); // Don't leak the shader.
      return;
   }

   // Initialize buffer for program info log in case shader program fails
   char buf[1024];
   GLsizei blah;

   // Attach the newly created vertex and fragment shader to our shader programs
   glAttachShader(shader, vert_shader);
   glAttachShader(shader, frag_shader);

   // Link the new shader program to OpenGL
   glLinkProgram(shader);
   cerr << "Enabling fragment program: " << gluErrorString(glGetError()) << endl;
   glGetProgramInfoLog(shader, 1024, &blah, buf);
   cerr << buf;

   cerr << "Enabling program object" << endl;

   // Use our newly made shader program
   glUseProgram(shader);

   // Get the location of the uniform variables (this will be sampler2d in our
   // GLSL shader program)
   texture_uniform_pos = glGetUniformLocation(shader, "texture_map");
   normal_uniform_pos = glGetUniformLocation(shader, "normal_map");

   // Sets the texture map and normal maps for the program
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_map);
   glUniform1i(texture_uniform_pos, 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, normal_map);
   glUniform1i(normal_uniform_pos, 1);
}

// Draw all the objects in the scene
void draw_objects() {
    vector<Vertex> vertex_buffer;
    vector<Vertex> normal_buffer;
    vector<Vertex> tangents;
    // u and v is x and y axis in texture respectively
    // In order of top right (1, 1), bottom right (1, 0), bottom left (0, 0) and top left (0, 1)
    GLfloat texture_buffer[] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, // top right, bottom right, bottom left
        0.0, 0.0, 0.0, 1.0, 1.0, 1.0}; // bottom left, top left, top right
    
    // Push a copy of the current Modelview Matrix onto the Stack
    glPushMatrix();

    // Vertices that define a square plane (using 2 triangular faces)
    // Face 1
    vertex_buffer.push_back(Vertex(1.0, 1.0, 0.0));
    vertex_buffer.push_back(Vertex(1.0, -1.0, 0.0));
    vertex_buffer.push_back(Vertex(-1.0, -1.0, 0.0));

    // Face 2
    vertex_buffer.push_back(Vertex(-1.0, -1.0, 0.0));
    vertex_buffer.push_back(Vertex(-1.0, 1.0, 0.0));
    vertex_buffer.push_back(Vertex(1.0, 1.0, 0.0));

    // Defines the normals for each vertex in this plane, which is all (0.0, 0.0, 1.0) which is all xy-plane
    for (int i = 0; i < 6; i++) {
        normal_buffer.push_back(Vertex(0.0, 0.0, 1.0));
    }

    // Set the pointer to the vertex buffer array of the object being rendered
    glVertexPointer(3, GL_FLOAT, 0, &vertex_buffer[0]);

    // Set the pointer to the normal buffer array of the object being rendered
    glNormalPointer(GL_FLOAT, 0, &normal_buffer[0]);

    // Set the pointer to the texture coordinate buffer array of the plane being rendered
    glTexCoordPointer(2, GL_FLOAT, 0, &texture_buffer[0]);

    // Draw the plane using GL_TRIANGLES
    int buffer_size = vertex_buffer.size();
    glDrawArrays(GL_TRIANGLES, 0, buffer_size);

    // Pop a copy of the current Modelview Matrix off the Stack
    glPopMatrix();
}

// Handle mouse events when mouse is pressed
void mouse_pressed(int button, int state, int x, int y) {
    // If left mouse button is pressed down then ...
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Retrieve position of the mouse
        start_x = x;
        start_y = y;

        // Set is_pressed flag to true
        is_pressed = true;
    }
    // Else, if left mouse button is released, then set the last rotation
    // to be the current rotation, and reset the current rotation to be the i
    // dentity matrix
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        // Set is_pressed flag to false
        is_pressed = false;
        last_rotation = get_current_rotation();
        curr_rotation = Quaternion_identity();
    }
}

// Handle mouse events when mouse is moved
void mouse_moved(int x, int y) {
    if (is_pressed) {
        // Modify the current x- and y- view angle
        curr_x = x;
        curr_y = y;

        // Avoid computing the rotation matrix if the starting and ending coordinates are the same,
        // this causes the rotation matrix to have nan's in them and crashes the program
        if (start_x != curr_x || start_y != curr_y) {
            curr_rotation = compute_rotation_Quaternion(curr_x, curr_y, start_x, start_y);
        }
        
        // Re-render our scene
        glutPostRedisplay();
    }
}

// Handle key events when a key is pressed
void key_pressed(unsigned char key, int x, int y) {
    // Quitting the program
    if(key == 'q')
    {
        exit(0);
    }
}

// Main function where the parsing is done and everything comes together
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./opengl_texture_renderer <color_texture.png> <normal_map.png>\n");
    }
    else {
        // Always initialize the width and height of the window to 800
        xres = 800;
        yres = 800;

        // Parse the texture filenames
        const char* color_texture_filename = argv[1];
        const char* normal_map_filename = argv[2];

        // Renders the scene
        glutInit(&argc, argv);

        // Initialize OpenGL display with double, RGB and depth buffers
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

        // Initialize window size
        glutInitWindowSize(xres, yres);

        // Set top-left corner of window to be (0, 0)
        glutInitWindowPosition(0, 0);

        // Create window with name "Shader"
        glutCreateWindow("Texture shader");

        // Call our init function
        init();

        cerr << "Loading textures\n" << endl;
        // Load the textures, if loading fails then exit program
        if (!(texture_map = readpng(color_texture_filename))) {
            cerr << "Failed to load texture map\n";
            exit(1);
        }
            
        if (!(normal_map = readpng(normal_map_filename))) {
            cerr << "Failed to load normal map\n";
            exit(1);
        }

        // Load in our shader programs
        vert_filename = "src_texture/vertex_program.glsl";
        frag_filename = "src_texture/fragment_program.glsl";
        read_shaders();
        
        // Set OpenGL display function to our display function
        glutDisplayFunc(display);

        // Set OpenGL reshape function to our reshape function
        glutReshapeFunc(reshape);

        // Set OpenGL mouse press handler to be our mouse_pressed function
        glutMouseFunc(mouse_pressed);

        // Set OpenGL mouse motion handler to be our mouse_moved function
        glutMotionFunc(mouse_moved);

        // Set OpenGL keyboard handler to be our key_pressed function
        glutKeyboardFunc(key_pressed);

        // Run our event processing loop
        glutMainLoop();   
    }
}
