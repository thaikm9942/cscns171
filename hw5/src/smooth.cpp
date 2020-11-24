// Includes for OpenGL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Includes for Eigen library and parser.h and quaternion.h and halfedge.h
#include "../Eigen/Dense"
#include "../Eigen/Sparse"
#include "../include/parser.h"
#include "../include/quaternion.h"
#include "../include/implicit_fairing.h"

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

/* Defining prototype functions we want to write for OpenGL */

void init(void);
void reshape(int width, int height);
void display(void);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

///////////////////////////////////////////////////////////////////////////////////////////////////

// Keeps track of the initial mouse positions and the ending mouse positions
int start_x, start_y, curr_x, curr_y;

// Keeps track of the last arcball rotation and the current arcball rotation
Quaternion last_rotation, curr_rotation;

// Scene should be a global variable
Scene scene;

// Timestep used for implicit fairing
double timestep;

// Controls the keyboard interactions to zoom in and zoom out of the screen
const float step_size = 0.2;
float x_view_angle = 0, y_view_angle = 0;

// Boolean flag to keep track of whether or not a mouse button is pressed or released
bool is_pressed = false;

// Boolean flag to indicate whether we are in wireframe mode
bool wireframe_mode = false;

// Name of the shader program and the vertex shader program filename and the fragment
// shader program filename
static GLenum shader;
static string vert_filename, frag_filename;

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

// Converts a vertex from screen coordinates to NDC coordinates
Vertex screen2ndc(float sx, float sy, int xres, int yres) {
    float ndc_x = (float) (2.0 * sx) / xres - 1.0;
    float ndc_y = (float) (scene.yres - sy) * 2.0 / yres - 1.0;
    float squared_sum = ndc_x * ndc_x + ndc_y * ndc_y;
    if (squared_sum > 1) {
        return Vertex(ndc_x, ndc_y, 0.0);
    }
    float ndc_z = abs(sqrt(1 - (squared_sum)));
    return Vertex(ndc_x, ndc_y, ndc_z);
}

/*
 * Helper functions for axis-angled rotational Arcball implementation. These functions
 * are currectly obsolete due to a more computationally efficient Quaternion implementation.
 */

// This function constructs a CCW rotation matrix given a vector u and an angle theta (in radians)
Eigen::Matrix4f create_rotation_matrix(float u[3], float theta) {
    // Normalize the given vector
    normalize(u);

    // Initialize the rotation matrix
    Eigen::Matrix4f r;

    // Constructing the first row values
    float r11 = u[0] * u[0] + (1 - u[0] * u[0]) * cos(theta);
    float r12 = u[0] * u[1] * (1 - cos(theta)) - u[2] * sin(theta);
    float r13 = u[0] * u[2] * (1 - cos(theta)) + u[1] * sin(theta);

    // Constructing the second row values
    float r21 = u[1] * u[0] * (1 - cos(theta)) + u[2] * sin(theta);
    float r22 = u[1] * u[1] + (1 - u[1] * u[1]) * cos(theta);
    float r23 = u[1] * u[2] * (1 - cos(theta)) - u[0] * sin(theta);

    // Constructing the third row values
    float r31 = u[2] * u[0] * (1 - cos(theta)) - u[1] * sin(theta);
    float r32 = u[2] * u[1] * (1 - cos(theta)) + u[0] * sin(theta);
    float r33 = u[2] * u[2] + (1 - u[2] * u[2]) * cos(theta);

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
Eigen::Matrix4f compute_rotation_matrix(float curr_x, float curr_y, float start_x, float start_y) {
    // Convert both starting coordinates and ending coordinates from screen to NDC
    Vertex curr_ndc = screen2ndc(curr_x, curr_y, scene.xres, scene.yres);
    Vertex start_ndc = screen2ndc(start_x, start_y, scene.xres, scene.yres);

    // Calculate the axis of rotation using cross product
    Vertex axis = cross(start_ndc, curr_ndc);
    float u[3] = {axis.x, axis.y, axis.z};

    // Compute the angle of rotation
    float arg = dot(start_ndc, curr_ndc) / (norm(start_ndc) * norm(curr_ndc));
    float theta = acosf(fmin(1.0, arg));

    return create_rotation_matrix(u, theta);
}

/*
 * Helper functions for Quaternion Arcball implementation. These functions
 * are currectly obsolete due to a more computationally efficient Quaternion implementation.
 */

// Get the current Arcball Quaternion representing the current rotation matrix
Quaternion get_current_rotation() {
    return curr_rotation.dot(last_rotation);
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

   cerr << "making fragment shader\n";
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
    Vertex curr_ndc = screen2ndc(curr_x, curr_y, scene.xres, scene.yres);
    Vertex start_ndc = screen2ndc(start_x, start_y, scene.xres, scene.yres);

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
    // Enables Gouraud Shading
    glShadeModel(GL_SMOOTH);

    // Enables backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enables depth buffering
    glEnable(GL_DEPTH_TEST);

    // Enables auto-normalization of vectors
    glEnable(GL_NORMALIZE);
    
    // Enables vertex buffer array and normal buffer array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Initialize the Arcball rotation matrices as identity Quaternions
    last_rotation = Quaternion_identity();
    curr_rotation = Quaternion_identity();

    // Set the matrix modified to be the projection matrix
    glMatrixMode(GL_PROJECTION);

    // Load the identity matrix as the projection matrix
    glLoadIdentity();

    // Create the perspective projection matrix using frustum parameters
    Perspective perspective = scene.perspective;
    glFrustum(perspective.left, perspective.right, 
        perspective.bottom, perspective.top, 
        perspective.near, perspective.far);

    // Set the matrix modified to be the main matrix
    glMatrixMode(GL_MODELVIEW);

    // Initialize the lights
    init_lights();
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

// Main "display" function
void display(void) {
    // Reset color buffer and depth buffer array
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the Modelview matrix to be the Identity matrix
    glLoadIdentity();

    // Remember to apply inverse camera transforms BEFORE the actual
    // geometric transforms due to OpenGL Post-Multiplication behavior

    // Apply inverse camera transform, including rotation AND then translation
    Camera camera = scene.camera;
    glRotatef(-rad2deg(camera.angle), camera.orientation[0], camera.orientation[1], camera.orientation[2]);
    glTranslatef(-camera.position[0], -camera.position[1], -camera.position[2]);

    // Converts the Quaternion to rotation matrix and apply this Arcball rotation to all points
    Quaternion arcball_quar = get_current_rotation();

    // Normalize the Quaternion to be a unit Quaternion
    arcball_quar.normalize();
    Eigen::Matrix4f arcball_rotation = quar2rot(arcball_quar);
    glMultMatrixf(arcball_rotation.data());

    // Set up the lights
    set_lights();

    // Draw objects
    draw_objects();

    // Swap the active and off-screen buffers with one another
    glutSwapBuffers();
}

// Initialize the light sources in the scene
void init_lights() {
    // Enables lighting calculations in OpenGL
    glEnable(GL_LIGHTING);

    // Retrieves all light sources
    vector<Light> light_sources = scene.light_sources;

    // Iterate through all light sources in OpenGL
    for (int i = 0; i < light_sources.size(); i++) {
        Light curr = light_sources[i];

        // Set up the current light ID in the OpenGL
        int light_id = GL_LIGHT0 + i;

        // Enables the current light in OpenGL
        glEnable(light_id);

        // Initialize the reflectance values for the current light. Since
        // our lights do not have such values, we can just set them
        // to be the color value
        glLightfv(light_id, GL_AMBIENT, curr.color);
        glLightfv(light_id, GL_DIFFUSE, curr.color);
        glLightfv(light_id, GL_SPECULAR, curr.color);

        // Set the attenuation constant of the light
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, curr.attenuation_k);
    }
}

// Sets the light sources position in the scene
void set_lights() {
    for (int i = 0; i < scene.light_sources.size(); i++) {
        Light curr = scene.light_sources[i];

        // Set the current light id
        int light_id = GL_LIGHT0 + i;

        // Set the current light position
        glLightfv(light_id, GL_POSITION, curr.position);
    }
}

// Draw all the objects in the scene
void draw_objects() {
    vector<Labeled_Object> objects = scene.objects;

    for (int i = 0; i < objects.size(); i++) {
        Object object = objects[i].obj;

        // Push a copy of the current Modelview Matrix onto the Stack
        glPushMatrix();

        for (int j = 0; j < object.transforms.size(); j++) {
            Transform_Set transform_set = object.transforms[j];

            // Have to iterate backwards since we want to make the calls to 
            // the transformation in REVERSE order considering OpenGL uses 
            for (int k = transform_set.transform_set.size() - 1; k >= 0; k--) {
                Transform transform = transform_set.transform_set[k];

                // Runs the correct OpenGL transformation based on the type of transformation
                switch (transform.type) {
                    case TRANSLATION:
                        glTranslatef(transform.parameters[0], transform.parameters[1], transform.parameters[2]);
                        break;
                    case ROTATION:
                        glRotatef(rad2deg(transform.angle), transform.parameters[0], transform.parameters[1], transform.parameters[2]);
                        break;
                    case SCALING:
                        glScalef(transform.parameters[0], transform.parameters[1], transform.parameters[2]);
                        break;
                }
            }
        }

        // Set the material properties of the object being rendered
        glMaterialfv(GL_FRONT, GL_AMBIENT, object.material.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, object.material.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, object.material.specular);
        glMaterialf(GL_FRONT, GL_SHININESS, object.material.shininess);

        // Set the pointer to the vertex buffer array of the object being rendered
        glVertexPointer(3, GL_FLOAT, 0, &object.vertex_buffer[0]);

        // Set the pointer to the normal buffer array of the object being rendered
        glNormalPointer(GL_FLOAT, 0, &object.normal_buffer[0]);

        int buffer_size = object.vertex_buffer.size();
        // If not wireframe mode, draw the vertices using GL_TRIANGLE
        if (!wireframe_mode) {
            glDrawArrays(GL_TRIANGLES, 0, buffer_size);
        }
        else {
            // Else, render using lines
            for (int j = 0; j < buffer_size; j += 3) {
                glDrawArrays(GL_LINE_LOOP, j, 3);
            }
        }

        // Retrieve the Modelview Matrix pretransformation of an object
        glPopMatrix();
    }
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

// Function to update the vertex and normal buffers after implicit fairing process is ran
void update_buffers() {
    for (int i = 0; i < scene.objects.size(); i++) {
        // Retrieves the mesh data for this object
        Mesh_Data* mesh_data = scene.objects[i].mesh_data;
        // Allocate new pointers to the store halfedge vertices and halfedge faces
        vector<HEV*> *hevs = new vector<HEV*>;
        vector<HEF*> *hefs = new vector<HEF*>;

        // Build the halfedge data structure representation of this object
        build_HE(mesh_data, hevs, hefs);

        // Solve the Poisson equation and updates the new vertex coordinates for each vertex after
        // smoothing
        solve(hevs, timestep);

        // Compute the NEW vertex normals for each vertex in the halfedge data structure,
        // need to ignore the first vertex since that's NULL
        for (int j = 1; j < hevs->size(); j++) {
            compute_vertex_normal(hevs->at(j));
        }

        // Clear out the old vertex buffer and normal buffer of the object so we can update them
        scene.objects[i].obj.vertex_buffer.clear();
        scene.objects[i].obj.normal_buffer.clear();

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
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v1->x, v1->y, v1->z));
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v2->x, v2->y, v2->z));
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v3->x, v3->y, v3->z));

            // Add their corresponding vertex normals into the normal buffer
            scene.objects[i].obj.add_normal_to_buffer(v1->normal);
            scene.objects[i].obj.add_normal_to_buffer(v2->normal);
            scene.objects[i].obj.add_normal_to_buffer(v3->normal);
        }

        // Delete the mesh data used to calculate our normals
        delete_HE(hevs, hefs);
    }
}

// Handle key events when a key is pressed
void key_pressed(unsigned char key, int x, int y) {
    // Quitting the program
    if(key == 'q')
    {
        exit(0);
    }
    // Toggle wireframe mode
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        // Re-render the scene
        glutPostRedisplay();
    }
    // Toggle implicit fairing when pressed
    else if(key == 'i')
    {
        cout << "Smoothing the mesh with timestep h = " << timestep << "\n";
        // Update the buffers with the new smoothed out vertices
        update_buffers();

        // Multiply the timestep by 2 for the next smoothing phase in case we press it again
        timestep *= 2;

        // Re-render the scene
        glutPostRedisplay();
    }
}

// Function to fill in the vertex and normal buffers after parsing the scene files using the halfedge data structure
void fill_buffers() {
    for (int i = 0; i < scene.objects.size(); i++) {
        // Retrieves the mesh data for this object
        Mesh_Data* mesh_data = scene.objects[i].mesh_data;
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
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v1->x, v1->y, v1->z));
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v2->x, v2->y, v2->z));
            scene.objects[i].obj.add_vertex_to_buffer(Vertex(v3->x, v3->y, v3->z));

            // Add their corresponding vertex normals into the normal buffer
            scene.objects[i].obj.add_normal_to_buffer(v1->normal);
            scene.objects[i].obj.add_normal_to_buffer(v2->normal);
            scene.objects[i].obj.add_normal_to_buffer(v3->normal);
        }

        // Delete the mesh data used to calculate our normals
        delete_HE(hevs, hefs);
    }
}

// Main function where the parsing is done and everything comes together
int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: ./smooth [scene_description_file.txt] [xres] [yres] [h]\n");
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
            // Width of the window screen
            int width = atoi(argv[2]);

            // Height of the window screen
            int height = atoi(argv[3]);

            // Timestep
            timestep = atof(argv[4]);

            // Initialize a map to store label with its associated Object
            map<string, Object> untransformed;

            // Initialize scene
            string line;        
            // Parsing text and loading all data into their appropriate data structures
            while (getline(ifs, line)) {
                vector<string> tokens = strsplit(line, ' ');

                // If there's only 1 token and the only token matches "camera:", then
                // create the scene with camera and perspective setup
                if (tokens.size() == 1 && tokens[0].compare(string("camera:")) == 0)  {
                    scene = create_scene(ifs, width, height);
                }
                // If the line starts with "objects:", then the
                // next few lines consist of .obj files that need to be parsed
                // and turned into Objects
                else if (tokens.size() == 1 && tokens[0].compare(string("objects:")) == 0) {
                    while(getline(ifs, line) && !line.empty()) {
                        tokens = strsplit(line, ' ');
                        // Creates filename by appending the correct data path
                        string filename = string("data/").append(tokens[1]);

                        // Creates the labeled object with the associated label
                        untransformed[tokens[0]] = create_object(filename.c_str());
                    }
                }
                // If the line does not start with "objects:", then the next few lines contain 
                // the transformations for the object with the corresponding label
                else if (tokens.size() == 1 && tokens[0].compare(string("objects:")) != 0) {
                    string label = tokens[0];

                    // Creates a copy of the labeled object
                    Object obj = untransformed[label];
                    // Pass in the filestream to parse the materials for the object
                    Material m = create_material(ifs);
                    obj.material = m;
                    // Pass in the filestream to parse the transformations for the object
                    Transform_Set t = create_transformation(ifs);
                    obj.transforms.push_back(t);

                    // Create Mesh_Data associated with object
                    Mesh_Data* mesh_data = create_mesh_data(obj);

                    // Add the object to the scene
                    scene.add_labeled_object(obj, label, mesh_data);             
                }  
            }

            // Fill in the vertex and normal buffers
            fill_buffers();

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

            // Load in our shader programs
            vert_filename = "src/vertex_phong.glsl";
            frag_filename = "src/fragment_phong.glsl";
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
}
