#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include <Eigen/Dense>

using namespace Eigen;
/*
 * FUNCTION DEFINITIONS
 */

Matrix4d create_rotation_matrix(double u[3], double theta);

#endif // #ifndef __TRANSFORM_H__