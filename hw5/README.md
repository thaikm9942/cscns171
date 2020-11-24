# Assignment 5
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose. 

All implementations of classes and functions in the header files are written in
`*.cpp` files under `/src`. 

To create the `smooth` program, simply run `make` on the terminal.
 
To run the `OpenGL` program on a specific scene text file, run 
`./smooth data/scene_*.txt xres yres h` on the terminal, 
where `xres` and `yres` are the resolutions for the window screen to display the images, and h is the timestep used to smooth out the resulting mesh via implicit fairing.

To trigger implicit fairing, press `'i'` and a message will appear saying that the mesh is being smoothed at the given timestep. The scene is then re-rendered after a few seconds.

## Part 1
Many data structures and the OpenGL demo from Assignment 4 Part 1 have been imported to this assignment, with a few major modifications. The `Face` class from Assignment 2 has been reimplemented to support the new half edge data structure implementation, and instead of the provided structs, such as `Vec3f`, `Vertex` and `Face`, the provided halfedge implementation has been modified to use the `Vertex` and `Face` class in `object.h` and `object.cpp`. The `Mesh_Data` struct from `structs.h` have been moved into `object.h` for convenience.

Majority of the parsing procedures are implemented in `parser.cpp`. Most of the parsing
functions are imported from last week's assignment. A notable change is compared to Assignment 4, vertex and normal buffers are no longer populated during the parsing of the file. The check for a vertex normal is also removed since we now have to manually compute the vertex normals for each vertex later on using the halfedge data structure. Instead, a vector of `Vertex` and `Face` objects inside the `Object` class are populated accordingly, and these will be used later on generate our `Mesh_Data`. The details of this implementation can be read in `create_mesh_data` in `halfedge.h` and `halfedge.c`. Each `Mesh_Data*` is now stored in the `Labeled_Object` struct under `scene.h` for convenience of accessing later on.

A few new functions have been defined in the provided `halfedge.h` header file to assist with computing the face normals. Namely, functions such as `compute_face_normal` and `compute_face_area` are used to compute the face normal and face area of the face adjacent to the given halfedge accordingly. Then, these values are used to compute the vertex normal of a given halfedge vertex in `compute_vertex_normal`, and the final, normalized vertex normal is stored within the `HEV` struct itself. The implementation of the vertex normal computation follows closely the pseudo code given in the Lecture Notes for Assignment 5.

Finally, after parsing the scene files, the vertex buffers and normal buffers are populated accordingly in `fill_buffers` under our OpenGL demo file `smooth.cpp`. For each object in the `Scene`, the object's mesh data is retrieved, and the halfedge representation of this object is generated using the proved `build_HE` function in `halfedge.h`. Then, for each halfedge vertex, we compute the vertex normal and store it within the halfedge vertex struct. Then, we iterate through each halfedge face, retrieves the 3 vertices that form the face, and add their vertex coordinates and vertex normals to the vertex buffer and normal buffer of the `Object` in order. The rendering of the final scene is carried over from the previous assignments.


## Part 2
For the next few parts, almost all of our functions that have to do with implicit fairing are located under `implicit_fairing.h` and `implicit_fairing.cpp`.

Our main function of interest here for building the `F` Laplacian matrix is the function `build_F_operator` in `implicit_fairing.cpp`. The algorithm for building the `F` operator follows closely the implementation for building the `B` operator in the lecture notes, with a few caveats:

1. We have that Delta_i = rho = 1 / 2A * sum_j (cot(alpha_j) + cot(beta_j)) (phi_j - phi_i). Since our summation involves both a phi_j and phi_i term, in order to build a matrix Delta such that we can solve for the phi vector, we will have to expand the sum out: 1 / 2A * sum_j (cot(alpha_j) + cot(beta_j)) phi_j - 1 / 2A * sum_j (cot(alpha_j) + cot(beta_j))

2. Looking at our expanded term, considering that the i is the current vertex we are looking at, and j is each adjacent vertex. For every phi_j in the phi_vector, our corresponding j-th entry in row i of the matrix Delta has coefficient C_ij = 1 / 2A * (cot(alpha_j) + cot(beta_j)). For every phi_i in the phi_vector, our corresponding i-th entry in the row i (or the diagonal entries) has coefficient C_ii = 1 / 2A * sum_j (cot(alpha_j) + cot(beta)). This means that, when we use our halfedge data structure to iterate and construct our matrix delta, for every halfedge vertex i (and row i), its coefficient at the i-th entry in row i is the sum of the cotangent sum term over all adjacent vertices j, where as for every vertex j adjacent to i, its coefficient at the j-th entry in row i is just the sum of the cotangent sum. We then divide the coefficients with 2 times the sum of the neighboring area of vertex i accordingly

3. With this knowledge in mind, we can follow the pseudocode for constructing the B operator and make a few adjustments in setting the entries to construct our Delta matrix. We also initialize an identity matrix of dimensions (number of vertices x number of vertices), and our final F matrix is (I - h * Delta). Further details (and comments) on the implementations can be read within the `build_F_operator` function. 

4. Many functions were written to help facilitate the building of the F operator, namely: `compute_sum_neighbor_area` computes the sum of the incident faces' areas given a vertex; `compute_cotan_sum` computes the cotangent sum of the angles facing the edge ij, where i is the vertex we're at and j is the adjacent vertex from i we're looking at. These functions are implemented and documented in `implicit_fairing.cpp` and `halfedge.cpp`.

## Part 3
The actual solving for `x_h, y_h` and `z_h` is done in the `solve` function in `implicit_fairing.cpp`. The `solve` function mimics closely the pseudocode for the `solve` function for the `B` operator in the Lecture Notes for Assignment 5 (Section 3). Our rho vectors for the initial `x`, `y` and `z` coordinates for all vertices can be obtained from our vector of halfedge vertices (which is provided as a pointer and generated earlier on from Part 1). The new vertex coordinates for each halfedge vertex is then updated accordingly at the end after solving for our phi vector.

## Part 4
The implicit fairing process occurs in the `update_buffers` function in our OpenGL demo file `smooth.cpp`. The idea is that, we solve for the new smoothed coordinates for halfedge vertex given our timestep parameter (which is parsed and stored as a global coordinate in `smooth.cpp`) and these are stored within the halfedge vertices themselves. Then, the new vertex normals are recomputed. Since our vertex buffers and normal buffers currently store the old coordinates, we must then clear both of them and re-add the new vertex coordinates and vertex normals. The implicit fairing process is triggered by pressing the `'i'` key on the keyboard, and each time this is pressed, the global timestep value is doubled and the scene is re-rendered.
