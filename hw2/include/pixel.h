#ifndef __PIXEL_H__
#define __PIXEL_H__

/* 
 * This header file defines the Pixel2D and the RGB_Color class
 * which are used to draw a Pixel in a given yres x xres grid
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class represents a RGB color using 3 integer values
class RGB_Color {  
    public:
        // The intensity values for the RGB color
        int r_, g_, b_;

        // Default constructor that initializes the default RGB color
        // which is black
        RGB_Color() : r_(0), g_(0), b_(0) {}

        // Constructor that takes in a given RGB color
        RGB_Color(int r, int g, int b) : r_(r), g_(g), b_(b) {}
};

// Constants defining specific colors using RGB_Color class
const RGB_Color YELLOW = RGB_Color(255, 255, 0);
const RGB_Color DARK_RED = RGB_Color(128, 0, 0);
const RGB_Color WHITE = RGB_Color(255, 255, 255);
const RGB_Color BLACK = RGB_Color(0, 0, 0);

// Max intensity for any RGB_Color value
const int MAX_INTENSITY = 255;

// This class represents a Pixel in a 2D drawing grid
class Pixel2D {
    public:
        // The row and column number of a Pixel in the grid
        int y_, x_;
        RGB_Color color_;
        
        // Default constructor for a Pixel at (0, 0) with the default color WHITE
        Pixel2D() : y_(0), x_(0), color_(WHITE) {};

        // Constructor that takes in the (row, column) of the Pixel and its
        // given color
        Pixel2D(int y, int x, RGB_Color color) : y_(y), x_(x), color_(color) {};
};

#endif // #ifndef __PIXEL_H__