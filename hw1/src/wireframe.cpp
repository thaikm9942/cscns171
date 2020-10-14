#include "../include/transform.h"
#include "../include/parser.h"
#include "../include/object.h"
#include <string.h>
#include <stdlib.h>

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

// Find and return (a copy of) the object with the correct label
Labeled_Object find_object_with_label(vector<Labeled_Object> elems, string label) 
{
    for (int i = 0; i < elems.size(); i++) {
        Labeled_Object elem = elems[i];
        if (elem.label.compare(label) == 0) {
            return elem;
        }
    }
    throw "Object with label not found";
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 2) {
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
            vector<Labeled_Object> untransformed;
            Scene scene;
            string line;
            while (getline(ifs, line)) {
                fprintf(stderr, "line: %s\n", line.c_str());
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

                        // Creates the labeled object
                        Labeled_Object l_obj = {tokens[0], create_object(filename.c_str())};

                        untransformed.push_back(l_obj);
                    }
                }
                // If the line does not start with "objects:", then the next few lines contain 
                // the transformations for the object with the corresponding label
                else if (tokens.size() == 1 && tokens[0].compare(string("objects:")) != 0) {
                    string label = tokens[0];
                    // Pass in the filestream to parse the transformations for the labeled object
                    Transformation t = create_transformation(ifs);
                    Labeled_Object elem = find_object_with_label(untransformed, label);
                    elem.obj.transform = t;
                    scene.add_labeled_object(elem);
                }  
            }

            // This line is used to print out the entire scene
            // and is used for debugging
            scene.print_scene();
        }


    }
}