# Assignment 4
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose. 

All implementations of classes and functions in the header files are written in
`*.cpp` files under `/src` or under `/src_texture`.

To create the `opengl_renderer` program for Part 1, simply run `make opengl_renderer` on the terminal.

To run the `OpenGL` program on a specific scene text file, run 
`./opengl_renderer [data/scene_*.txt] [xres] [yres] [mode]` on the terminal, 
where `xres` and `yres` are the resolutions for the window screen to display the images, and `mode` determines whether Gouraud shading or Phong shading will be used to render the scene.

To create the `opengl_texture_renderer` program responsible for texture and normal mapping for Part 2, simply run `make opengl_texture_render` on the terminal.

To run the `OpenGL` texture rendering program on a specific image and its corresponding normal mapping file, run 
`./opengl_texture_renderer [color_texture.png] [normal_map.png]` on the terminal, 
where `color_texture.png` is the texture image file used to determine the color of the flat square surface, and `normal_map.png` is the normal mapping file of the corresponding texture image file used to render the depths of the texture image.

To make both programs for Part 1 and Part 2, run `make all`.

## Part 1
Many data structures from Assignment 3 and the OpenGL demo has been imported over to this part of the assignment. All files of interest is in `/include` and `/src`

The vertex and fragment GLSL shader files written to implement Phong shading are `fragment_phong.glsl` and` vertex_phong.glsl`. The vertex shader for the Phong shading algorithm is fairly simple: all we need to do is apply our transformations for our vertex and normal vectors from world to camera space. The bulk of the work for this part is in the fragment shader. The basic idea is to implement the `lighting` algorithm from Assignment 2, which can be understood in more details from the pseudocode given in the Lecture Notes for Assignment 2 or through the comments in `fragment_phong.glsl`.

The `opengl_renderer.cpp` file has been modified from last week to incorporate the loading the shader depending on which mode we are in. If `mode` is `0`, then we don't need to load any shaders since our default shading is the Gouraud shading. If `mode` is `1`, then our Phong GLSL shaders is loaded inside the `main` function. Our main function of interest is `read_shaders`. This function was imported from the given GLSL shader demo and is fairly straightforward. 

## Part 2
All files of interest is under `/src_texture` to accommodate for different code structures compared to the `opengl_renderer` program. Note that our program here is `opengl_texture_renderer`.

Our basic set up is: we create a square surface of coordinates `(-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0)`, on which we will render our texture onto. We then fill up our vertex buffer with 2 triangles forming this surface (for rendering later). Our vertex normals for all coordinates is simply the unit normal along the z-axis `(0, 0, 1)`. The texture coordinates for each coordinates is then calculated by hand using some math (we want to align our texture onto the surface using the uv axes). A single light source is hardcoded with position `(0.0, 0.0, 3.0)`, such that our light source is directly facing our square surface. Our camera is currently placed at the origin. For some reason, when I move the camera further away from the plane, the square plane disappears, and I wasn't able to figure out the cause of this.

The `read_shaders` function from Part 1 is imported over, with a few additional modifications, such as setting our texture and normal map files and their corresponding global uniform variables in our shaders in order to use them for the texture rendering. The texture and normal map files themselves are loaded in the `main` function using the provided functions in `readpng.cpp` given in the GLSL shader demo before the shaders are loaded.

Our GLSL shader files of interest are `vertex_program.glsl` and `fragment_program.glsl`. In the vertex shader, the per-vertex texture coordinate is set to be interpolated by the fragment shader. The surface normal is the retrieved from the normal mapping for this vertex and mapped from `[0, 1]` to `[-1, 1]`. Then, the tangent (which is hardcoded since our normal vector of the surface is just `(0, 0, 1)`) and bitangent (the cross product of the surface normal vector and the tangent) is calculated. Our final light direction is then calculate after converting the camera and light sources position into surface space. In the fragment shader, the texture color value and normal map coordinate is retrieved from our uniform `texture_map` and `normal_map` variables (which was loaded by our CPU). Then, using the simple diffuse lighting model provided, we calculate the color value from the surface normal coordinates. Then, then final color is equal to the product of the texture color and the normal mapping color values.

