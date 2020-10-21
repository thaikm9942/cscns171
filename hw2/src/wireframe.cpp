#include "../include/parser.h"
#include "../include/scene.h"
#include <string.h>
#include <stdlib.h>
#include <map>

// Main function
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./wireframe <scene_description_file.txt> xres yres\n");
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
            // Store the width and height of the pixel grid
            int width = atoi(argv[2]);
            int height = atoi(argv[3]);

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
                    scene = create_camera_and_perspective(ifs);
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
                    // Pass in the filestream to parse the transformations for the labeled object
                    Transformation t = create_transformation(ifs);
                    Object obj = untransformed[label];
                    obj.transform = t;
                    scene.add_labeled_object(obj, label);
                }  
            }

            // Apply all transformations to the scene
            scene.apply_transformations();

            // Return pixels to be output into a .ppm file
            set<tuple<int, int>> pixels = scene.get_pixels(width, height);
            
            // Prints that this is a PPM file
            cout << "P3\n";

            // Prints xres and yres
            cout << width << ' ' << height << "\n";

            // Prints the max intensity
            cout << MAX_INTENSITY << "\n";

            // Iterate through all rows and columns and draw the pixels
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    // If the current pixel in the grid is found in the set of
                    // of pixels to be drawn, then draw it using a WHITE color
                    if(pixels.find(tuple<int, int>(x, y)) != pixels.end()) {
                        cout << WHITE.r_ << ' ' << WHITE.g_ << ' ' << WHITE.b_ << "\n";

                    }
                    // Else, draw it with BLACK color
                    else {
                        cout << BLACK.r_ << ' ' << BLACK.g_ << ' ' << BLACK.b_ << "\n";                 
                    }
                }
            }
        }
    }
}