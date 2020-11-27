# Assignment 6
Name: Thai Khong

## Part 1
### The Single Spring Pendulum - Part1/Single_Spring_Pendulum
All code was written in `single_spring_pendulum_demo.cpp`. To create the `single_pendulum` program, simply run `make` on the terminal. To run the program, run `./single_pendulum [xres] [yres] [x_start] [y_start]` on the same terminal, where `xres` and `yres` are the window resolutions for the program, and `x_start` and `y_start` is the starting coordinates of the pendulum mass. To run the demo provided in the assignment, simply run `./single_pendulum 500 500 4 -2'.

### The Double Spring Pendulum - Part1/Double_Spring_Pendulum
All code was written in `double_spring_pendulum_demo.cpp`. To create the `double_pendulum` program, simply run `make` on the terminal. To run the program, run `./double_pendulum [xres] [yres] [x_start_1] [y_start_1] [x_start_2] [y_start_2]` on the same terminal, where `xres` and `yres` are the window resolutions for the program, and `x_start_1` and `y_start_1` and `x_start_2` and `y_start_2` is the starting coordinates of the first and second pendulum mass respectively. To run the demo provided in the assignment, simply run `./double_pendulum 500 500 0 -1 4 2'.

### Elasticity - Part1/Elasticity
The `Eigen` library has been imported from the previous homework assignment. All code was written in `elastic_demo.cpp`. To create the `simulate` program, simply run `make` on the terminal. To run the program, run `./simulate [object.obj]` on the same terminal, where `object.obj` is the object file we want to simulate the elasticity demo on. To run the demo provided in the assignment, simply run `./simulate man.obj`.

## Part 2
### I-Bar Animator - Part2/I_Bar
All necessary header files are included in the `/include` folder.
Each header file has a block of comment explaining its purpose. 

All implementations of classes and functions in the header files are written in
`*.cpp` files under `/src`. 

To create the `./keyframe` program, simply run `make`.

To run an I-Bar animation, simply run `./keyframe [test_script.script]`, where `test_script.script` is one of the script files given. To step through each frame in the I-Bar animation, simply press `n`. A text keeping track of which frame we are in has been drawn at the top right corner of the window.

For the purpose of this assignment, many classes were imported (and revamped) from the previous assignments, such as the `Quaternion` and `Vertex` classes. A new class called `Frame` has been implemented to support the animation demo. Implementation details can be found under `frame.h` and `frame.cpp` files.

The main idea is that each `Frame` contains a set of geometric transformations (translation, scaling and rotation) either parsed from the script files or interpolated between any pair of keyframes using the Cat-Rom splines. These geometric transformations are then applied before the actual I-Bar object is drawn in the `drawIBar` function under `keyframe.cpp`. Each frame also has a corresponding `frame_id`, which is later on used to generate the number of interpolated frames we want to generate between any 2 keyframes. Another boolean, `is_keyframe`, is used to determine whether the frame is a keyframe or not. This helps us with text display at the top right of the demo window to let us know whether we are seeing a keyframe or an interpolated frame.

In the demo file `keyframe.cpp`, a global variable called `all_frames`, which keeps track of all the `Frame`s in the animation, is used to store all the frames in the full animation, and another global variable, an integer called `curr_frame_id`, is used to keep track of the current `Frame` we are in.

The perspective projection matrix and the inverse camera transform is hardcoded as provided in the assignment.

The parsing of the script file is done in the `main` function. Each translation, scaling and rotation transformation set is stored in a `Frame` object. Since the frames provided in the script file are keyframes, all keyframes are stored initially in a vector of `Frame` inside the `main` function. The first keyframe is added to the end of this vector of keyframes to make generating the last few interpolating frames easier. Then, the complete, interpolated frames of the I-Bar animation are generated in the `generate_interpolated_frames` function.

The `generate_interpolated_frames` is implemented as follows:
1. For every pair of consecutive keyframes, we obtain the four 'control' keyframes as needed to generate our Cat-Rom spline. 
2. Then, the number of interpolated frames we need to generate between 2 keyframes is calculated using the difference between the 2 id numbers.
3. The step size (for our `u` parameter) is just 1.0 divided by the number of interpolated frames we want
4. A separate variable is used to keep track of the interpolated frame ids.
5. We add the first keyframe into our set of frames for the animation `all_frames`, and then add every interpolated frame, iterating from `u = step_size` to `u = 1.0 - step_size.` The reason why we don't interpolate for `u = 0` and `u = 1` is because these parameters already corresponding to the first and second keyframe.
6. Then, the `find_interpolated_frame` function is called using the four `control` keyframes and corresponding `u` parameter. This function basically interpolates the translation, scaling vector and the rotation Quaternion for the interpolated frame using the Cat-Rom spline function provided in the lecture notes for Assignment 6. The functions `find_interpolated_translation`, `find_interpolated_scaling`, `find_interpolated_quaternion` and `find_interpolated_rotation` accomplish exactly this. 7. The actual value interpolation is done in the `find_interpolated_value` function, which takes a column vector `p` corresponding to the values of the specific component of our 4 control values and the step parameter `u` and returns the final interpolated component using the function `f(u) = u * B * p` via matrix and vector multiplication.

### Bunny Frames - Part2/Bunny_Frames
All necessary header files are included in the `/include` folder.
Each header file has a block of comment explaining its purpose. 

All implementations of classes and functions in the header files are written in
`*.cpp` files under `/src`. 

To create the `./keyframe` program, simply run `make`.

To run the bunny smoothing animation, simply run `./keyframe keyframes/bunny00.obj keyframes/bunny05.obj keyframes/bunny10.obj keyframes/bunny15.obj keyframes/bunny20.obj`. The provided `.obj` files are essentially the keyframes in the animation. To step through each frame in the Bunny Smoothing animation, simply press `n`. A text keeping track of which frame we are in has been drawn at the top right corner of the window.

For the purpose of this assignment, many classes were imported (and revamped) from the previous assignments, such as the halfedge data structure for HW 5, our `Vertex`, `Face` and `Object` classes. The `Frame` has been revamped from the I-Bar animation to support the animation demo, notably, each frame now contains a pointer to an `Object`. Implementation details can be found under `frame.h` and `frame.cpp` files.

The main idea is that each `Frame`, compared to the I-Bar animation, now contains a set of vertices of the Bunny object to be drawn. However, we want to store a pointer to the `Object` since we also need the `Face`s of the Bunny object, and later on, we would ideally want to generate the vertex normals (using our halfedge data structure implementation from HW 5) and fill up our vertex and normal buffers for `OpenGL` to draw the actually Bunny object corresponding to the frame.

In the demo file `keyframe.cpp`, a global variable called `all_frames`, which keeps track of all the `Frame`s in the animation, is used to store all the frames in the full animation, and another global variable, an integer called `curr_frame_id`, is used to keep track of the current `Frame` we are in. This is similar to our I-Bar animation.

Since we are essentially only drawing one bunny, the perspective projection matrix, the inverse camera transform, and the light sources for the scene, the material of the Bunny object, and the geometric transformations were hardcoded in using the `scene_bunny.txt` file from HW 5.

The parsing of the .obj files is done in the `main` function. Our `Object` parsing functions were imported from the `parser.cpp` file of HW 5 to parse the `.obj` file correctly. Each `Frame` object now stores an `Object` to be drawn. Similar to our I-Bar animation, all keyframes with the keyframe objects are stored initially in a vector of `Frame` inside the `main` function. Then, the complete, interpolated frames of the Bunny Smoothing animation are generated in the `generate_interpolated_frames` function. The logic of this function is similar to the one in the I-Bar animation, except now we are interpolating the coordinates of each vertex for the new frame using the coordinates of that vertex in the keyframe objects. This can be found in `find_interpolated_frame` and `find_interpolated_vertex`. Then, after we have all of our frames for the set of all frames, we calculate the vertex normals for each Frame-Object pair, and then fill up the vertex and normal buffers for `OpenGL` to draw the object correctly. This is done in `fill_buffers`.


 