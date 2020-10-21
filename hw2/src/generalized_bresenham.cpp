#include "../include/bresenham.h"
#include <math.h>

//////////////////////////////
///    HELPER FUNCTIONS    ///
//////////////////////////////

// This function handles the line drawing using Bresenham's Line Algorithm
// for all small positive slope cases (0 <= m <= 1). In particular, this handles
// the 1st and 5th octant. This is the integer-only version which ensures
// fast and efficient computation.
// Precondition: x0 < x1, y0 < y1 (dx > 0, dy > 0)
void small_pos_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels) {
    int eps = 0;
    int y = y0;
    int dx = x1 - x0; // should be positive
    int dy = y1 - y0; // should also be positive
    
    // Iterate in increasing x order
    for (int x = x0; x < x1; x++) {
        // Add the pixel to be drawn into the existing pixels set
        pixels.insert(tuple<int, int>(x, y));

        // If the point at (x + 1, y) is closer,
        // then increment the error by dy
        if ((eps + dy) << 1 < dx) {
            eps += dy;
        }
        // Else, the point at (x + 1, y + 1) is closer,
        // increment y by 1 (since y0 < y1)
        else {
            eps += dy - dx;
            y += 1;
        }
    }
}

// This function handles the line drawing using Bresenham's Line Algorithm
// for all small negative slope cases (-1 <= m <= 0). In particular, this handles
// the 4th and 8th octant. This is the integer-only version which ensures
// fast and efficient computation.
// Precondition: x0 < x1, y0 > y1 (dx > 0, dy < 0)
void small_neg_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels) {
    int eps = 0;
    int y = y0;
    int dx = x1 - x0; // should be positive
    int dy = y1 - y0; // should be negative
    
    // Iterate in increasing x order
    for (int x = x0; x < x1; x++) {
        // Add the pixel to be drawn into the existing pixels vector
        pixels.insert(tuple<int, int>(x, y));

        // If the point at (x + 1, y) is closer,
        // then increment the error by dy
        if ((eps + dy) << 1 > -dx) {
            eps += dy;
        }
        // Else, the point at (x + 1, y - 1) is closer,
        // decrement y by 1 (since y0 > y1)
        else {
            eps += dy + dx;
            y -= 1;
        }
    }
}

// This function handles the line drawing using Bresenham's Line Algorithm
// for all large positive slope cases (1 <= m <= INFINITY). In particular, this handles
// the 2nd and 6th octant. This is the integer-only version which ensures
// fast and efficient computation.
// Precondition: x0 < x1, y0 < y1 (dx > 0, dy > 0)
void large_pos_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels) {
    int eps = 0;
    int x = x0;
    int dx = x1 - x0; // should be positive
    int dy = y1 - y0; // should also be positive
    
    // Iterate in increasing y order
    for (int y = y0; y <= y1; y++) {
        // Add the pixel to be drawn into the existing pixels vector
        pixels.insert(tuple<int, int>(x, y));

        // If the point at (x, y + 1) is closer,
        // then increment the error by dx
        if ((eps + dx) << 1 < dy) {
            eps += dx;
        }
        // Else, the point at (x + 1, y + 1) is closer,
        // increment x by 1 (since x0 < x1)
        else {
            eps += dx - dy;
            x += 1;
        }
    }
}

// This function handles the line drawing using Bresenham's Line Algorithm
// for all large negative slope cases (-INFINITY <= m <= -1). In particular, this handles
// the 3rd and 7th octant. This is the integer-only version which ensures
// fast and efficient computation.
// Precondition: x0 > x1, y0 < y1 (dx < 0, dy > 0)
void large_neg_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels) {
    int eps = 0;
    int x = x0;
    int dx = x1 - x0; // should be negative
    int dy = y1 - y0; // should be positive
    
    // Iterate in increasing y order
    for (int y = y0; y <= y1; y++) {
        // Add the pixel to be drawn into the existing pixels vector
        pixels.insert(tuple<int, int>(x, y));

        // If the point at (x, y + 1) is closer,
        // then increment the error by dx
        if ((eps + dx) << 1 > -dy) {
            eps += dx;
        }
        // Else, the point at (x - 1, y + 1) is closer,
        // decrement x by 1 (since x0 > x1)
        else {
            eps += dx + dy;
            x -= 1;
        }
    }
}

//////////////////////////////
///    MAIN FUNCTIONS      ///
//////////////////////////////
void generalized_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    // Positive slope cases
    if (dx * dy > 0) {
        // If both dx and dy < 0, we flip the endpoints
        if (dx < 0 && dy < 0) {
            int temp_x = x0;
            int temp_y = y0;
            x0 = x1;
            y0 = y1;
            x1 = temp_x;
            y1 = temp_y;
            dx *= -1;
            dy *= -1;
        }

        // 1st and 5th octant since 0 <= dy / dx <= 1
        if (dx >= dy) {
            small_pos_bresenham(x0, y0, x1, y1, pixels);
        }
        // 2nd and 6th octant since 1 < dy / dx < INFINITY
        else {
            large_pos_bresenham(x0, y0, x1, y1, pixels);
        } 
    }
    // Negative slope cases (dx > 0 and dy < 0) or (dx < 0 and dy > 0)
    else {
        // 4th and 8th quadrant since -1 <= dy / dx <= 0 (dx > 0, dy < 0)
        if (abs(dx) >= abs(dy)) {
            // If dy < 0 and dx > 0, then (x1, y1) lies in the 8th quadrants,
            // flip endpoints to be in 4th quadrant 
            if (dy > 0 && dx < 0) {
                int temp_x = x0;
                int temp_y = y0;
                x0 = x1;
                y0 = y1;
                x1 = temp_x;
                y1 = temp_y;
                dx *= -1;
                dy *= -1;
            }
            small_neg_bresenham(x0, y0, x1, y1, pixels);
        }
        // 3rd and 7th quadrant since -INFINITY <= dy / dx < -1 (dx < 0, dx > 0)
        else {
            // If dy < 0 and dx > 0, then (x1, y1) lies in the 7th quadrant,
            // flip endpoints to be in the 3rd quadrant 
            if (dy < 0 && dx > 0) {
                int temp_x = x0;
                int temp_y = y0;
                x0 = x1;
                y0 = y1;
                x1 = temp_x;
                y1 = temp_y;
                dx *= -1;
                dy *= -1;
            }
            large_neg_bresenham(x0, y0, x1, y1, pixels);
        }
    }
}