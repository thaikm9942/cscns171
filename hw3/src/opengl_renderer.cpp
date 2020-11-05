/* The following 2 headers contain all the main functions, data structures, and
 * variables that allow for OpenGL development.
 */
#include <GL/glew.h>
#include <GL/glut.h>

/* You will almost always want to include the math library. For those that do
 * not know, the '_USE_MATH_DEFINES' line allows you to use the syntax 'M_PI'
 * to represent pi to double precision in C++. OpenGL works in degrees for
 * angles, so converting between degrees and radians is a common task when
 * working in OpenGL.
 *
 * Besides the use of 'M_PI', the trigometric functions also show up a lot in
 * graphics computations.
 */
#include "../include/parser.h"
#include <math.h>
#define _USE_MATH_DEFINES

// C++ data structures
#include <vector>
#include <map>

using namespace std;

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

/* The following are parameters for creating an interactive first-person camera
 * view of the scene. The variables will make more sense when explained in
 * context, so you should just look at the 'mousePressed', 'mouseMoved', and
 * 'keyPressed' functions for the details.
 */

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Scene should be a global variable
Scene scene;

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


void reshape(int width, int height) {
    // Sets the minimum screen size to be 1 x 1
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;

    // Sets up Viewport to convert NDC to screen coordinates
    glViewport(0, 0, width, height);

    // Update mouse interface parameters
    Perspective perspective = scene.perspective;
    mouse_scale_x = (float) (perspective.right - perspective.left) / (float) width;
    mouse_scale_y = (float) (perspective.top - perspective.bottom) / (float) height;

    // Re-renders the scene after resizing
    glutPostRedisplay();
}

void display(void) {
    // Reset color buffer and depth buffer array
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the Modelview matrix to be the Identity matrix
    glLoadIdentity();

    // Rotate the camera from its default orientation based on
    // user's mouse interactions
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);

    // Remember to apply inverse camera transforms BEFORE the actual
    // geometric transforms due to OpenGL Post-Multiplication behavior

    // Apply inverse camera transform, including rotation AND then translation
    Camera camera = scene.camera;
    glRotatef(-rad2deg(camera.angle), camera.orientation[0], camera.orientation[1], camera.orientation[2]);
    glTranslatef(-camera.position[0], -camera.position[1], -camera.position[2]);
    
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
                transform.print_transformation();

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
        mouse_x = x;
        mouse_y = y;

        // Set is_pressed flag to true
        is_pressed = true;
    }
    // Else, if left mouse button is released up
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        // Set is_pressed flag to false
        is_pressed = false;
    }
}

// Handle mouse events when mouse is moved
void mouse_moved(int x, int y) {
    if (is_pressed) {
        // Modify the current x- and y- view angle
        x_view_angle += ((float) x - (float) mouse_x) * mouse_scale_x * x_view_step;
        float temp_y_view_angle = y_view_angle + 
            ((float) y - (float) mouse_y) * mouse_scale_y * y_view_step;
        y_view_angle = (temp_y_view_angle > 90 || temp_y_view_angle < -90) ?
                       y_view_angle : temp_y_view_angle;

        // Update current mouse (x, y)-position
        mouse_x = x;
        mouse_y = y;

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
    // Toggle wireframe mode
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        // Re-render the scene
        glutPostRedisplay();
    }
    else
    {
        float x_view_rad = deg2rad(x_view_angle);
        
        // 'w' for step forward
        if(key == 'w')
        {
            scene.camera.position[0] += step_size * sin(x_view_rad);
            scene.camera.position[2] -= step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        // 'a' for step left
        else if(key == 'a')
        {
            scene.camera.position[0] -= step_size * cos(x_view_rad);
            scene.camera.position[2] -= step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
        // 's' for step backward
        else if(key == 's')
        {
            scene.camera.position[0] -= step_size * sin(x_view_rad);
            scene.camera.position[2] += step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        // 'd' to step right
        else if(key == 'd')
        {
            scene.camera.position[0] += step_size * cos(x_view_rad);
            scene.camera.position[2] += step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./opengl_renderer <scene_description_file.txt> xres yres\n");
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
            // Number of columns in pixel grid - xres
            int width = atoi(argv[2]);

            // Number of rows in pixel grid - yres
            int height = atoi(argv[3]);

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
                    scene.add_labeled_object(obj, label);                 
                }  
            }

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
