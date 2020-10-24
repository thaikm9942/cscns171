# Assignment 1
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose. 

All implementations of functions in the header files are written in
*.cpp files under `/src`. 

To create the `wireframe` program, simply run `make` on the terminal. 
To run the program on a specific scene text file, run 
`./wireframe data/scene_*.txt xres yres` on the terminal, 
where `xres` and `yres` are the resolutions for the pixel grid to output the 
contents of the .ppm files. To convert the output to a .png file, run 
`./wireframe data/scene_*.txt xres yres | convert - my_image_name.png`.

## Part 1
Majority of the parsing procedures are implemented in `parser.cpp`. Most of the parsing
functions are imported from last week's assignment, notably `create_transformation`
and `create_object`, which handles the parsing of .obj files and blocks of text
involving geometric transformation. A new function called `create_camera_and_perspective`
to parse blocks of text containing the camera orientation and perspective projection 
matrix. The scene code is parsed under `wireframe.cpp`.

## Part 2
3 new classes have been created to accomodate this task:
`Camera`, `Perspective` and `Scene` can be found under
`scene.cpp`. In particular, the transformation matrices for
for converting between world coordinates and camera coordinates
and the perspective projection matrix can are implemented under
`compute_camera_transform()` and `compute_perspective_matrix()`
methods respectively in `scene.cpp`.

## Part 3
These conversions are primarily done in the `apply_transformations()`
method of the `Scene` class. For every `Object` and every `Vertex`
in the `Object`, the geometric transformations are applied first,
then the inverse camera transform is applied to convert world-space
to camera-space coordinates, and finally, the perspective matrix
is applied to convert camera-space coordinates to NDC coordinates.
During transformation, all homogeneous coordinates are normalized by
the `w` component, and this is done in every transformation through
the `to_cartesian()` method through the `Vertex` class under 
`object.cpp`.

## Part 4
All NDC coordinates are converted to screen coordinates through
the `Object` method `get_screen_coordinates(int xres, int yres)`
in `object.cpp`, which takes in the x- and y-resolution of the
pixel grid. In the process of transforming all NDC coordinates
to pixels, the original NDC coordinates are checked (namely,
the x- and y-coordinate) if they are contained within the
NDC cube using an axis-aligned approach through the `Vertex`
method `is_contained()` in `object.cpp`. For each pair of vertices in
a face, if their NDC coordinates are contained in the cube, then
then a line is drawn using the generalized Bresenham's Line Algorithm.

## Part 5
Most of explanations for the generalized Bresenham's line algorithm can
be found under `generalized_bresenham.cpp`. The main idea is that,
extending from the first octant's version, we consider all 8 possible
octants' version. We first notice that some octants are essentially
the same octant but with endpoints flip. For example, the 1st and
5th octant are essentially the same implementation with endpoints 
flipped (5th octant has `dx < 0` and `dy < 0` on top of `0 <= m <= 1>`).
This allows us to reduce the total number of cases to `4` cases:
For positive slopes, we have `0 <= m < = 1` or `1 < m < INFINITY`,
and for negative slopes, we have `-1 <= m <= 0` or `-INFINITY < m < -1`.
For the `1 < m < INFINITY` case (or steep positive slope), instead of
interating over increasing `x`, we iterate over increasing `y` instead
and swap all `dx` with `dy` variables (this can be derived using
the same steps from the first octant derivation). For the `-1 <= m <= 0`
case, instead of incrementing `y` in the first octant derivation, we
DECREMENT instead (since the slope is negative). For the 
`-INFINITY < m < -1`, this is the same case as the large steep 
positive slope, and only require some clever swapping instead. 
The main generalized version is then implemented and calls one of the 4 cases 
by checking the preconditions and swapping the endpoints as necessary. 
This is found in `generalized_bresenham()` function.

## Part 6
See start of file.