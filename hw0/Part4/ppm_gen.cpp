#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

// This class represents a RGB color using 3 integer values that 
// respectively denotes the red, green and blue intensity
class RGB_Color {
    public:
        int r_, g_, b_;

        // Default constructor that initializes the default RGB color
        // which is black
        RGB_Color() : r_(0.0), g_(0.0), b_(0.0) {}

        // Constructor that takes in a given RGB color
        RGB_Color(int r, int g, int b) : r_(r), g_(g), b_(b) {}
};

// Constants defining specific colors using RGB_Color class
const RGB_Color YELLOW = RGB_Color(255, 255, 0);
const RGB_Color DARK_RED = RGB_Color(128, 0, 0);

// Max intensity for any RGB_Color value
const int MAX_INTENSITY = 255;

// Main function
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./ppm_test xres yres\n");
    }
    else {
        // Initialize the number of rows and columns (based on yres and xres)
        int cols = atoi(argv[1]);
        int rows = atoi(argv[2]);      

        // Sets the diameter for the circle based on whichever is the smaller dimension
        // between xres and yres
        int diameter;
        if (rows <= cols) {
            diameter = rows;
        }
        else {
            diameter = cols;
        }

        // Prints that this is a PPM file
        cout << "P3\n";

        // Prints xres and yres
        cout << cols << ' ' << rows << "\n";

        // Prints the max intensity
        cout << MAX_INTENSITY << "\n";

        // Set the center of the circle to be the middle of the grid.
        // As an approximation, this should be at (cols / 2, rows / 2)
        int center_x = cols / 2;
        int center_y = rows / 2;

        int r2 = (diameter / 2) * (diameter / 2);

        // Iterate through all rows and columns and draw the pixels
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                // Calculates the distance between the current pixel
                // and the pixel at the center of the circle
                int dx = x - center_x;
                int dy = y - center_y;

                // Calculate the distance between this pixel to the
                // center of the circle pixel
                int d = dx * dx + dy * dy;

                // Check if the pixel is inside of the circle. If it is,
                // color it yellow. Else, color it red.
                if (d <= (diameter / 2) * (diameter / 2)) {
                    cout << YELLOW.r_ << ' ' << YELLOW.g_ << ' ' << YELLOW.b_ << "\n";
                }
                else {
                    cout << DARK_RED.r_ << ' ' << DARK_RED.g_ << ' ' << DARK_RED.b_ << "\n";                 
                }
            }
        }
    }     
}