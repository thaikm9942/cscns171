#include "../include/parser.h"
#include "../include/scene.h"
#include <string.h>
#include <stdlib.h>
#include <map>

// Max intensity for any RGB_Color value
const int MAX_INTENSITY = 255;

// Runs the correct shading on the scene based on the mode
void run_shading(Scene &scene, vector<vector<Color>> &pixels, vector<vector<double>> &buffer, int mode) {
    if (mode == 0) {
        scene_gouraud_shading(scene, pixels, buffer);
    }
    else if (mode == 1) {
        scene_phong_shading(scene, pixels, buffer);
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: ./shaded <scene_description_file.txt> xres yres mode\n");
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
            int mode = atoi(argv[4]);

            // Initialize a map to store label with its associated Object
            map<string, Object> untransformed;

            // Initialize scene
            Scene scene;
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
                    Object obj = untransformed[label];
                    // Pass in the filestream to parse the materials for the labeled object
                    Material m = create_material(ifs);
                    obj.m_ = m;
                    // Pass in the filestream to parse the transformations for the labeled object
                    Transformation t = create_transformation(ifs);
                    obj.t_ = t;
                    scene.add_labeled_object(obj, label);
                    
                }  

            }

            // scene.print_scene();

            // Initializes our rasterization grid used to do buffering
            vector<vector<Color>> pixels(height);
            for (int i = 0; i < pixels.size(); i++) {
                pixels[i] = vector<Color>(width);
            }

            // Initializes a buffer grid with the same dimensions as our raster grid
            vector<vector<double>> buffer(height);
            for (int i = 0; i < buffer.size(); i++) {
                buffer[i] = vector<double>(width);
                for (int j = 0; j < buffer[i].size(); j++) {
                    buffer[i][j] = INFINITY;
                }
            }

            // Apply all geometric transformations to the scene, our 
            // vertices are still in world-space coordinates
            scene.apply_transformations();

            run_shading(scene, pixels, buffer, mode);

            /*
             * The below code section outputs the PPM file
             */

            // Prints that this is a PPM file
            cout << "P3\n";

            // Prints xres and yres
            cout << width << ' ' << height << "\n";

            // Prints the max intensity
            cout << MAX_INTENSITY << "\n";

            // Iterate through all rows and columns and draw the pixels
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Color c = pixels[y][x];
                    cout <<  round(c.r_ * MAX_INTENSITY) << ' ' << round(c.g_ * MAX_INTENSITY) << ' ' << round(c.b_ * MAX_INTENSITY) << '\n';
                }
            }
        }
    }
}