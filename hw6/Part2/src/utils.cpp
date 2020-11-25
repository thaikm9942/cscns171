#include "../include/utils.h"
#include <math.h>

//////////////////////////////
///    USEFUL FUNCTIONS    ///
//////////////////////////////

/*
 * Useful helper functions for general calculation purposes
 */

void normalize(float u[3]) {
    float norm = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
    u[0] = u[0] / norm;
    u[1] = u[1] / norm;
    u[2] = u[2] / norm;
}

// Helper function to convert an angle from degree to radian
float deg2rad(float degs)
{
    return degs * M_PI / 180.0;
}

// Helper function to convert an angle from radians to degree
float rad2deg(float rads)
{
    return rads * 180.0 / M_PI;
}