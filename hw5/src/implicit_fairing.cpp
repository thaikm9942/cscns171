#include "../include/implicit_fairing.h"
#include <vector>

// Assigns each vertex in our vector of halfedge vertices to an index
void index_vertices(vector<HEV*> *hevs) {
    // Starts at index 1 due to 1-indexed vertices
    for (int i = 1; i < hevs->size(); ++i) {
        hevs->at(i)->index = i;
    }
}

/* Compute the sum of cotangent term in the Laplacian operator
 *
 * @param i - the vertex we are currently looking at
 * @param j - the vertex the out edge is pointing towrads
 * @param m - the vertex whose edge mi and mj forms an angle opposite of the edge ij
 * @param n - the vertex whose edge ni and nj forms a another angle opposite of the edge ij
 * 
 * @return - the sum of cotangent of angles facing the edge ij
 */
float compute_cotan_sum(HEV* i, HEV* j, HEV* m, HEV* n) {
    // Vectors forming 1 of the opposite angles (alpha_j)
    Vertex mi = Vertex(i->x - m->x, i->y - m->y, i->z - m->z);
    Vertex mj = Vertex(j->x - m->x, j->y - m->y, j->z - m->z);

    // Vectors forming the other opposite angle (beta_j)
    Vertex ni = Vertex(i->x - n->x, i->y - n->y, i->z - n->z);
    Vertex nj = Vertex(j->x - n->x, j->y - n->y, j->z - n->z);

    // Cosine angle of 2 vectors is the dot product divided by the norm of each vector
    // So, we have cos(alpha) = mi * mj / (|mi| * |mj|)
    // Sine angle of 2 vectors is the magnitude of the cross product divided by the norm of each vector
    // So, we have sin(alpha) = |mi x mj| / (|mi| * |mj|)
    // Therefore, cot(alpha) = cos(alpha) / sin(alpha) = mi * mj / |mi x mj|
    float cot_alpha = dot(mi, mj) / norm(cross(mi, mj));

    // Similarly, we compute the cotangent of beta using the principle above
    float cot_beta = dot(ni, nj) / norm(cross(ni, nj));

    // Return the sum of the cotangents
    return cot_alpha + cot_beta;
}

// Threshold to check if the sum of neighboring area is close to 0
const float epsilon = 10e-6;

/*
 * Function that constructs our F matrix (Laplacian operator) such that we can solve for the new smoothed
 * coordinates for our vertices
 * 
 * @param *hevs - pointer to our vector of halfedge vertices
 * @param h - timestep
 */
Eigen::SparseMatrix<double> build_F_operator(vector<HEV*> *hevs, double h) {
    // Calculate the number of vertices - have to subtract 1 due to 1-indexing
    int size = hevs->size() - 1;

    // Initialize our sparse matrix to represent our F operator
    Eigen::SparseMatrix<double> F(size, size);

    // Initialize our identity matrix
    Eigen::SparseMatrix<double> I(size, size);
    I.setIdentity();
    I.makeCompressed();

    // Reserve room for 7 non-zeros per row of B (this is considering the number of adjacent vertices to a vertex)
    F.reserve(Eigen::VectorXi::Constant(size, 7));

    for(int i = 1; i < hevs->size(); ++i) {
        // Gets the current vertex
        HEV* hev = hevs->at(i); 
        // Gets the half edge corresponding to the current vertex
        HE* he = hev->out;

        // Compute the neighbor area sum for v_i
        float sum_neighbor_area = compute_sum_neighbor_area(hev);

        // If the sum neighbor area is really small, then just skip the i-th row completely (equivalent to setting
        // all entries on i-th row to 0)
        if (sum_neighbor_area <= epsilon) {
            continue;
        }

        // Sum of cotangent sum for (i, i)-th 
        float sum_cot_sum = 0.0;

        // Iterate through all adjacent vertices
        do {
            // Get index of adjacent vertex v_j to v_i
            int j = he->next->vertex->index;

            // Compute the cotangent sum term
            float cot_sum = compute_cotan_sum(he->vertex, he->next->vertex, he->flip->next->next->vertex, he->next->next->vertex);
            sum_cot_sum += cot_sum;

            // Fill the j-th column of the i-th row of our F matrix with the correct value (which is just the cotangent sum)
            double c_ij = cot_sum / (2.0 * sum_neighbor_area);
            F.insert(i-1, j-1) = c_ij;

            // Retrieves the next halfedge
            he = he->flip->next;
        }
        while (he != hev->out);

        // Fill the diagonal entry (i, i)-th of our F matrix with the negated SUM of cotangent sums.
        // More details is provided in the README.
        double c_ii = -sum_cot_sum / (2.0 * sum_neighbor_area);
        F.insert(i-1, i-1) = c_ii;
    }

    // Make Eigen store F efficiently
    F.makeCompressed();

    // Return the final F matrix
    return I - h * F;
}

// Function to solve
Eigen::VectorXd solve_phi(Eigen::SparseMatrix<double> sparse, Eigen::VectorXd rho_vector, int size) {
    // Initialize Eigen sparse solver
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int> > solver;

    // The following two lines essentially tailor our solver to our operator sparse matrix
    solver.analyzePattern(sparse);
    solver.factorize(sparse);

    // Initialize our vector representation of phi
    Eigen::VectorXd phi_vector(size);

    // Solve for our phi vector
    phi_vector = solver.solve(rho_vector);

    // Return the resulting phi vector
    return phi_vector;
}

// Function to solve the Poisson equation involving the F operator
void solve(vector<HEV*> *hevs, double h)
{
    // Index our vertices in the vector of halfedge vertices
    index_vertices(hevs);
    
    // Get our matrix representation of F
    Eigen::SparseMatrix<double> F = build_F_operator(hevs, h);

    // Size of our rho/phi vector (basically the number of vertices in the vector of halfedge vertices)
    int size = hevs->size() - 1;

    // Initialize the rho vector representation of all the x coordinates for each
    Eigen::VectorXd rho_x(size);
    Eigen::VectorXd rho_y(size);
    Eigen::VectorXd rho_z(size);

    // Initialize the rho vectors of x_0, y_0 and z_0 by copying the current x-, y- and z-coordinates from each vertex into
    // the corresponding rho vector
    for (int i = 1; i < hevs->size(); i++) {
        rho_x(i - 1) = hevs->at(i)->x;
        rho_y(i - 1) = hevs->at(i)->y;
        rho_z(i - 1) = hevs->at(i)->z;
    }

    // Solve for the new x_h, y_h and z_h coordinates after smoothing
    Eigen::VectorXd phi_x = solve_phi(F, rho_x, size);
    Eigen::VectorXd phi_y = solve_phi(F, rho_y, size);
    Eigen::VectorXd phi_z = solve_phi(F, rho_z, size);

    // Iterate over each halfedge vertex and set the new x, y, z coordinates to be the newly computed ones
    for (int i = 1; i < hevs->size(); i++) { // Start at index 1 due to NULL
        hevs->at(i)->x = phi_x(i - 1);
        hevs->at(i)->y = phi_y(i - 1);
        hevs->at(i)->z = phi_z(i - 1);
    }
}