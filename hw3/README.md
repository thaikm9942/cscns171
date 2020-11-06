# Assignment 3
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose. 

All implementations of classes and functions in the header files are written in
*.cpp files under `/src`. 

To create the `opengl_renderer` program, simply run `make` on the terminal. 
To run the `OpenGL` program on a specific scene text file, run 
`./opengl_renderer data/scene_*.txt xres yres` on the terminal, 
where `xres` and `yres` are the resolutions for the window screen to display the images.

## Part 1
Many data structures from Assignment 2 have been imported to this assignment, with a few major modifications. Namely, most class definitions have been retained with a few minor modifications (such as field renaming and additional constructors) and many functions that had to deal with rendering and shading have been removed to clean up code. 

The `Transformation` class from last week have been revamped. There are now 2 classes: `Transform`, which contains the parameters for a specific geometric transformation and its type (rotation, scaling or translation), and `Transform_Set`, which denotes a set of `Transform`. Many of the previous `Transformation` class functions have been rewritten in the `transform.cpp` file to accommodate these changes. This new definition of a `Transform` and `Transform_Set` class will allow us to correctly apply geometric transformations for an `Object` in the correct order. For additional details, see documentation in the header file `transform.h`

The `Object` class now contains both an array of vertices and vertex normals, AND a buffer array of vertex and vertex normals. This is to accommodate the OpenGL rendering function, which requires a buffer array of vertex and normals. The construction of these buffer arrays are handled in the parsing procedure below. The `Face` class is also now removed, since we no longer require the faces of an `Object` in order to render the image aside from the buffer arrays.

Majority of the parsing procedures are implemented in `parser.cpp`. Most of the parsing
functions are imported from last week's assignment. A notable change is instead of adding the `Face` when a `f` symbol is read, we now add the vertices and vertex normals corresponding to the face directly to the corresponding buffer arrays. The final parsing of the scene text files is assembled under `opengl_renderer.cpp`, before. 

A few prototype functions have been imported from the `OpenGL_Demo` file given in the lecture notes to the new program. The `Scene` object representing the scene is also now a global variable to make writing these prototype functions easier (such as accessing the `Perspective` or the `Camera` for their parameters)

## Part 2
The `Quaternion` class and its functions is defined and implemented in `quaternion.h` and `quaternion.cpp` respectively. Basic quaternion operations (such as adding, subtracting, multiplying, identity, ...) have been written in `quaternion.cpp`. Many other helper functions, notably `quar2rot` and `compute_rotation_quaternion` in `opengl_renderer.cpp` have been implemented to assist with the conversion between rotation matrix and quaternions. 2 global variables: `last_rotation` and `curr_rotation` now keep track of the rotation quaternions needed for the Arcball rotations. The mouse event handler and mouse motion handler from the `OpenGL_Demo` have been modified to closely match the Arcball algorithm pseudocode in the lecture notes. The actual Arcball rotation is handled in the `display` function between the inverse camera transform application AND the initialization of lights and drawing of objects.
