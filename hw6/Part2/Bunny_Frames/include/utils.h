#ifndef __UTILS_H__
#define __UTILS_H__

/*
 * Useful helper functions for general calculation purposes
 */

// Helper function to normalize a vector
void normalize(float u[3]);

// Helper function to convert an angle from degree to radian
float deg2rad(float degs);

// Helper function to convert an angle from radians to degree
float rad2deg(float rads);

#endif // #ifndef __UTILS_H__