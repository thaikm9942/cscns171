#ifndef __RASTERIZATION_H__
#define __RASTERIZATION_H__

#include "./scene.h"

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

/**
 * This function rasterizes a colored triangle formed by the 3 given
 * vertices.
 *
 * @param a_ndc a vertex in the triangle (in NDC coordinates)
 * @param b_ndc a vertex in the triangle
 * @param c_ndc a vertex in the triangle
 * @param c_a the rgb color value of vertex a
 * @param c_b the rgb color value of vertex b
 * @param c_c the rgb color value of vertex c
 * @param xres number of columns of the pixel grid
 * @param yres number of rows of the pixel grid
 * @param pixels the pixel grid storing Color values to be filled in
 * @param buffer the buffer grid (with the same dimensions as the pixel grid) 
 *  used to employ depth buffering. This grid should be initialized
 *  before passing in.
 */
void raster_colored_triangle(Vertex a_ndc, Vertex b_ndc, Vertex c_ndc, 
    Color c_a, Color c_b, Color c_c, int xres, int yres, 
    vector<vector<Color>> &pixels, vector<vector<double>> &buffer);

/**
 * This function implements the Gourad shading algorithm 
 *
 * @param a a vertex in the triangle
 * @param an the vertex normal associated with @param a
 * @param b a vertex in the triangle
 * @param bn the vertex normal associated with @param b
 * @param c a vertex in the triangle
 * @param cn the vertex normal associated with @param c
 * @param mat the material properties of the Object
 * @param scene the scene containing the Object
 * @param cam the Camera object containing the camera position and the camera transform
 * @param pixels the pixel grid storing Color values to be filled in
 * @param buffer the buffer grid (with the same dimensions as the pixel grid) 
 *  used to employ depth buffering. This grid should be initialized
 *  before passing in.
 */
void gourad_shading(Vertex a, Vertex an, Vertex b, Vertex bn, Vertex c, Vertex cn, 
    Material mat, Scene scene, 
    vector<vector<Color>> &pixels, vector<vector<double>> &buffer);
#endif // #ifndef __RASTERIZATION_H__