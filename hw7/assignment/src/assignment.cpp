#include "object.h"
#include "scene.h"

#include <iostream>

#include "image.h"

using namespace Eigen;
using namespace std;

const int MAX_ITERS = 10000;
const int XRES = 500;
const int YRES = 500;

// This function calculates the result of the general inside-outside superquadric function
// If result < 0, then the point is inside the superquadric object
// If result = 0, then the point is on the object's surface
// If result > 0, then the point is outside the superquadric object
double InsideOutsideFunc(double x, double y, double z, double e, double n) {
    return -1.0 + pow(z * z, 1.0 / n) + pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n);
}

/**
 * IOTest Code
 */

bool Superquadric::IOTest(const Vector3d &point) {
    // Since all transformations in Superquadric are world-to-body
    // transformations, we need to apply the inverse transforms
    // to convert our world-space point to body-space before
    // using our inside-outside function to check if it's inside
    // or outside the object

    // Defines the homogeneous coordinate for the point - w is just 1.0
    Vector4d pt = Vector4d(point(0), point(1), point(2), 1.0);
    
    for (auto it = transforms.rbegin(); it != transforms.rend(); it++) {
        // Get the inverse world-to-body transform matrix
        Matrix4d inverse_transform = (*it)->GetMatrix().inverse();
        pt = inverse_transform * pt;
    }

    // Plug the final transformed point into the generalized superquadric function
    double result = InsideOutsideFunc(pt(0), pt(1), pt(2), exp0, exp1);
    
    // If the result of the inside-outside function is < 0, then the coordinate
    // is inside the superquadric, return true.
    if (result < 0) {
        return true;
    }

    // Else, it's either outside the superquadric or on its surface, return false
    return false;
}

bool Assembly::IOTest(const Vector3d &point) {
    // Defines the homogeneous coordinate for the point - w is just 1.0
    Vector4d pt = Vector4d(point(0), point(1), point(2), 1.0);

    // Applies all inverse assembly transformations to the point to convert from
    // world-to-body space
    for (auto it = transforms.rbegin(); it != transforms.rend(); it++) {
        // Convert the point from world-to-body
        Matrix4d inverse_transform = (*it)->GetMatrix().inverse();
        pt = inverse_transform * pt;
    }

    for(auto &child : children) {
        bool inside = child->IOTest(Vector3d(pt(0), pt(1), pt(2)));

        // If one of children contains this point, then we know this point
        // is already inside the assembly
        if (inside) {
            return true;
        }
    }

    // Else, if none of the children contains this point, then the assembly
    // does not contain the point
    return false;
}

/**
 * Closest Intersection Code
 */

pair<double, Intersection> Superquadric::ClosestIntersection(const Ray &ray) {
    /**
     * PART 1
     * TODO: Implement a ray-superquadric intersection using Newton's method.
     *       Make sure to apply any transformations to the superquadric before
     *       performing Newton's method.
     */
    pair<double, Intersection> closest = make_pair(INFINITY, Intersection());
    return closest;
}

pair<double, Intersection> Assembly::ClosestIntersection(const Ray &ray) {
    /**
     * PART 1
     * TODO: Implement a ray-assembly intersection by recursively finding
     *       intersection with the assembly's children. Make sure to apply any
     *       transformations to the assembly before calling ClosestIntersection
     *       on the children.
     */
    pair<double, Intersection> closest = make_pair(INFINITY, Intersection());
    return closest;
}

/**
 * Raytracing Code
 */

void Scene::Raytrace() {
    Image img = Image(XRES, YRES);

    for (int i = 0; i < XRES; i++) {
        for (int j = 0; j < YRES; j++) {
            /**
             * PART 2
             * TODO: Implement raytracing using the code from the first part
             *       of the assignment. Set the correct color for each pixel
             *       here.
             */
            img.SetPixel(i, j, Vector3f::Ones());
        }
    }

    // Outputs the image.
    if (!img.SaveImage("rt.png")) {
        cerr << "Error: couldn't save PNG image" << std::endl;
    } else {
        cout << "Done!\n";
    }
}
