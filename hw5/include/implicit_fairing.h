#ifndef __IMPLICIT_FAIRING_H__
#define __IMPLICIT_FAIRING_H__

#include "./halfedge.h"
#include "../Eigen/Dense"
#include "../Eigen/Sparse"

using namespace std;

/* Our main function that solves the Poisson equation responsible to generate the new coordinates for each vertex
 * in the smoothed mesh via implicit fairing. Details of the implementation is provided more in the comments of the 
 * .cpp file
 * 
 * @param *hevs - a pointer to the list of halfedge vertices generated from our mesh data
 * @param h - the timestep that defines the rate at which we smooths our mesh
 */
void solve(vector<HEV*> *hevs, double h);

#endif // #ifndef __IMPLICIT_FAIRING_H__