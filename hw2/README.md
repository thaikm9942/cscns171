# Assignment 2
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose. 

All implementations of functions in the header files are written in
*.cpp files under `/src`. 

To create the `shaded` program, simply run `make` on the terminal. 
To run the program on a specific scene text file, run 
`./shaded data/scene_*.txt xres yres mode` on the terminal, 
where `xres` and `yres` are the resolutions for the pixel grid to output the 
contents of the .ppm files, and `mode` determines whether Gouraud or Phong
shading will be executed on the scene. To convert the output to a .png file, run 
`./shaded data/scene_*.txt xres yres mode | convert - my_image_name.png`.

## Part 1
Majority of the parsing procedures are implemented in `parser.cpp`. Most of the parsing
functions are imported from last week's assignment. Notable changes include the 
`create_light` and `create_material` functions, which parse the block of texts corresponding
to the light sources and the reflectance and shininess properties of an object. A few
minor changes were made to the `create_object` function to handle the addition of 
vertex normal indices. The final parsing of the text is assembled under `shaded.cpp`.
3 new classes have been created to accomodate this task: `Material` in `object.h`, `Light` 
and `Color` in `light.h`. Documentations are available in the respective header files.

## Part 2
The transformation matrix converting between the world coordinates and camera coordinates
and the perspective projection matrix was imported from last week's assignment and
can be found under `scene.h` in the `Perspective` and `Camera` classes. 
The actual transformation occurs in `apply_transformations` in `scene.cpp`, which
applies only the geometric transforms to the vertices.

## Part 3
This task is accomplished in `apply_transformations` as well, specifically in the
`transform_normals` function. The fourth column and fourth row of the geometric
transformation is completely disregarded (i.e, set to all `0`s in `make_normal_transform`
under `object.cpp`. The transformation of the normal vertices themselves occur in 
`get_transformed_normals` in `object.cpp` as well.

## Part 4
The lighting model algorithm is documented in `light.h` and is implemented
in the `lighting` function under `light.cpp`. The implementation follows closely the 
pseudocode laid out in Assignment 2's Lecture Notes.

## Part 5
The colored triangles rasterization algorithm is defined and documented in `rasterization.h`
header file and implemented in `rasterization.cpp` itself under `raster_colored_triangles`
function. The details of the implementation can be read in the comments. The implementation follows 
closely the pseudocode laid out in Assignment 2's Lecture Notes.

## Part 6
The Gouraud shading algorithm is defined and documented in `rasterization.h`
header file and implemented in `rasterization.cpp` itself under `gouraud_shading`
function. The details of the implementation can be read in the comments. The implementation 
follows closely the pseudocode laid out in Assignment 2's Lecture Notes.

## Part 7
The Phong shading algorithm is defined and documented in `rasterization.h`
header file and implemented in `rasterization.cpp` itself under `phong_shading`
function. The details of the implementation can be read in the comments. The implementation 
follows closely the pseudocode laid out in Assignment 2's Lecture Notes.

## Part 8
The pixel grid output code is done in `shaded.cpp` towards the end.
