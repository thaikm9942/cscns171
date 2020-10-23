#include "../include/rasterization.h"
#include <math.h>

// Function to check if the barycentric coordinates is contained within
// the triangle formed by the 3 vertices
bool in_triangle(double value) {
    return 0 <= value && value <= 1;
}

// Generalized function to compute the barycentric coordinates
double compute_function(double xi, double yi, double xj, double yj, double x, double y) {
    return (yi - yj) * x + (xj - xi) * y + xi * yj - xj * yi;
}

// Compute the alpha component of the barycentric coordinates of the point P = (x, y)
double compute_alpha(double xa, double ya, double xb, double yb, double xc, double yc, double x, double y) {
    // alpha = f_bc(x_p, y_p) / f_bc(x_a, y_a)
    // Here, i = b and j = c.
    return compute_function(xb, yb, xc, yc, x, y) / compute_function(xb, yb, xc, yc, xa, ya);
}

// Compute the beta component of the barycentric coordinates of the point P = (x, y)
double compute_beta(double xa, double ya, double xb, double yb, double xc, double yc, double x, double y) {
    // beta = f_ac(x_p, y_p) / f_ac(x_b, y_b)
    // Here, i = a and j = c.
    return compute_function(xa, ya, xc, yc, x, y) / compute_function(xa, ya, xc, yc, xb, yb);
}

// Compute the gamma component of the barycentric coordinates of the point P = (x, y)
double compute_gamma(double xa, double ya, double xb, double yb, double xc, double yc, double x, double y) {
    // gamma = f_ab(x_p, y_p) / f_ab(x_c, y_c)
    // Here, i = a and j = b.
    return compute_function(xa, ya, xb, yb, x, y) / compute_function(xa, ya, xb, yb, xc, yc);
}

// Implements the Gourad Shading algorithm
void gourad_shading(Vertex a, Vertex an, Vertex b, Vertex bn, Vertex c, Vertex cn, 
    Material mat, Scene scene,
    vector<vector<Color>> &pixels, vector<vector<double>> &buffer) {
    
    // Retrieves the Camera object and the light sources from the scene
    vector<Light> lights = scene.ls_;
    Camera cam = scene.cam_;

    // Finds the Color values of a, b and c
    Color c_a = lighting(a, an, mat, lights, cam.p_);
    Color c_b = lighting(b, bn, mat, lights, cam.p_);
    Color c_c = lighting(c, cn, mat, lights, cam.p_);

    // Converts a, b, c from world space coordinates to NDC coordinates
    Vertex a_ndc = scene.to_ndc_coordinates(a);
    Vertex b_ndc = scene.to_ndc_coordinates(b);
    Vertex c_ndc = scene.to_ndc_coordinates(c);

    raster_colored_triangle(a_ndc, b_ndc, c_ndc, c_a, c_b, c_c, scene.xres_, scene.yres_, pixels, buffer);
}

// Return a new color that is a linear combination of three colors given alpha, beta and gamma coefficients
Color add_colors(double alpha, double beta, double gamma, Color c_a, Color c_b, Color c_c) {
    double r_new = alpha * c_a.r_ + beta * c_b.r_ + gamma * c_c.r_;
    double g_new = alpha * c_a.g_ + beta * c_b.g_ + gamma * c_c.g_;
    double b_new = alpha * c_a.b_ + beta * c_b.b_ + gamma * c_c.b_;
    return Color(r_new, g_new, b_new);
}

// Implements the colored triangle rasterization algorithm
void raster_colored_triangle(Vertex a_ndc, Vertex b_ndc, Vertex c_ndc, 
    Color c_a, Color c_b, Color c_c, int xres, int yres,
    vector<vector<Color>> &pixels, vector<vector<double>> &buffer) {
    // Calculates the cross product of (c - b) and (a - b)
    Vertex cross_product = cross(subtract(c_ndc, b_ndc), subtract(a_ndc, b_ndc));

    // If the component of the cross product is negative, then
    // this is a back-facing triangle
    if (cross_product.z_ < 0) { return; }

    // Converts all NDC coordinates to screen coordinates
    Vertex sa = a_ndc.to_screen_coordinates(xres, yres);
    Vertex sb = b_ndc.to_screen_coordinates(xres, yres);
    Vertex sc = c_ndc.to_screen_coordinates(xres, yres);

    // Initialize coordinates for the bounding box
    int x_min = min(sa.x_, min(sb.x_, sc.x_));
    int x_max = max(sa.x_, max(sb.x_, sc.x_));
    int y_min = min(sa.y_, min(sb.y_, sc.y_));
    int y_max = max(sa.y_, max(sb.y_, sc.y_));

    // Iterate through all coordinates of the bounding box
    for (int x = x_min; x <= x_max; x++) {
        for (int y = y_min; y <= y_max; y++) {
            // Computes the barycentric coordinates for the point (x, y)
            double alpha = compute_alpha(sa.x_, sa.y_, sb.x_, sb.y_, sc.x_, sc.y_, x, y);
            double beta = compute_beta(sa.x_, sa.y_, sb.x_, sb.y_, sc.x_, sc.y_, x, y);
            double gamma = compute_gamma(sa.x_, sa.y_, sb.x_, sb.y_, sc.x_, sc.y_, x, y);

            // Checks if the barycentric coordinates of a point are within the triangle 
            // formed by the 3 vertices a, b, c
            if (in_triangle(alpha) && in_triangle(beta) && in_triangle(gamma)) {
                // Finds the NDC coordinates of the point defined by (x, y)
                Vertex p = add(add(dot(alpha, a_ndc), dot(beta, b_ndc)), dot(gamma, c_ndc));

                // If the point p is contained within the NDC cube and it is not blocked
                // by any points with a smaller z value, then find its color as
                // as barycentric coordinates of the colors of a, b and c and fill it
                // in the pixel grid
                if (p.is_contained() && p.z_ <= buffer[y][x]) {
                    buffer[y][x] = p.z_;  
                    Color c = add_colors(alpha, beta, gamma, c_a, c_b, c_c);
                    // printf("x: %d, y: %d\n", x, y);
                    // printf("c: %f %f %f\n", c.r_, c.g_, c.b_);
                    pixels[y][x] = c;
                }
            }
        }
    }
}