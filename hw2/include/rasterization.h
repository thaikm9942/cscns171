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
 * @param a first vertex in the triangle in world coordinates
 * @param b second vertex in the triangle in world coordinates
 * @param c third vertex in the triangle in world coordinates
 * @param an the vertex normal associated with @param a
 * @param bn the vertex normal associated with @param b
 * @param cn the vertex normal associated with @param c
 * @param mat pointer to the material properties of the Object
 * @param scene pointer to the scene containing the Object
 * @param pixels a pointer to the pixel grid storing Color values to be filled in
 * @param buffer a pointer to the buffer grid (with the same dimensions as the pixel grid) 
 *  used to employ depth buffering. This grid should be initialized
 *  before passing in.
 */
void phong_shading(Vertex a, Vertex b, Vertex c, Vertex an, Vertex bn, Vertex cn,
    Material &mat, Scene &scene, vector<vector<Color>> &pixels, vector<vector<double>> &buffer);

/**
 * This function implements the Gouraud shading algorithm 
 *
 * @param a first vertex in the triangle in world coordinates
 * @param an the vertex normal associated with @param a
 * @param b second vertex in the triangle in world coordinates
 * @param bn the vertex normal associated with @param b
 * @param c third vertex in the triangle in world coordinates
 * @param cn the vertex normal associated with @param c
 * @param mat a pointer to the material properties of the Object
 * @param scene a pointer to the scene containing the Object
 * @param pixels a pointer the pixel grid storing Color values to be filled in
 * @param buffer a pointer to the buffer grid (with the same dimensions as the pixel grid) 
 *  used to employ depth buffering. This grid should be initialized
 *  before passing in.
 */
void gouraud_shading(Vertex a, Vertex an, Vertex b, Vertex bn, Vertex c, Vertex cn, 
    Material &mat, Scene &scene, 
    vector<vector<Color>> &pixels, vector<vector<double>> &buffer);

#endif // #ifndef __RASTERIZATION_H__