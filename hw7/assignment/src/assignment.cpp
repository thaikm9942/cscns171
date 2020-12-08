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

inline int sign(float x) {
    return (x > 0) ? 1 : -1;
}

// Function to return the initial guess for our t_old Newton iterative solver
double GetInitialGuess(const Ray &ray) {
    Vector3d a_vec = ray.direction;
    Vector3d b_vec = ray.origin;

    // Calculate the coefficients of our quadratic equation
    double a = a_vec.transpose() * a_vec;
    double b = 2.0 * a_vec.transpose() * b_vec;
    double c = b_vec.transpose() * b_vec - 3;

    // Calculate delta term
    double delta = b * b - 4 * a * c;

    // If delta < 0, then the ray misses the bounding sphere completely, return INFINITY to indicate this 
    if (delta < 0) {
        return INFINITY;
    }

    // Solve the quadratic equations for our func
    double t1 = sign(b) * (-abs(b) - delta) / (2.0 * a);
    double t2 = sign(b) * (2.0 * c) / (-abs(b) - delta);

    // If b >= 0, then t1 = t_minus and t2 = t_plus. Else, t1 = t_plus and t2 = t_minus. 
    // We can flip t1 and t2 when b < 0 such that t1 = t_minus and t2 = t_plus is always true.
    if (b < 0) {
        double temp = t1;
        t1 = t2;
        t2 = temp;
    }

    // Now, we always have t1 = t_minus and t2 = t_plus
    // If both t values are positive, return t_minus (t1) since it's closer to the camera
    if (t1 > 0 && t2 > 0) {
        return t1;
    }
    // If both t values are negative, then the superquadric surface can't be seen (i.e, it's behind the camera)
    // return INFINITY to indicate this.
    else if (t1 < 0 && t2 < 0) {
        return INFINITY;
    }
    // If 1 positive and 1 negative, then return whichever's positive
    else {
        if (t1 > 0) {
            return t1;
        }
        else {
            return t2;
        }
    }

    // Return INFINITY otherwise
    return INFINITY;
}

// Computes the gradient of the inside-outside function at a given 3D point
Vector3d InsideOutsideGrad(double x, double y, double z, double e, double n) {
    double dx = (2.0 * x) * pow(x * x, 1.0 / e - 1.0) * pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n - 1.0);
    double dy = (2.0 * y) * pow(y * y, 1.0 / e - 1.0) * pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n - 1.0);
    double dz = (2.0 * z) * pow(z * z, 1.0 / n - 1.0);
    Vector3d grad(dx, dy, dz);
    return grad / n;
}

// Uses the Newton Iterative Solver to find the smallest t value such that g'(t) ~ 0
double NewtonIterativeSolver(Ray &ray, double e, double n) {
    // Threshold for stopping conditions - used to check when the gradient and our function
    // is close enough to 0 that we can stop
    double epsilon = 1e-6;

    // Solve for the initial guess of t using quadratic equations
    double t_old = GetInitialGuess(ray);

    // If our guess is negative infinity, then this ray misses the superquadric completely or
    // our object is behind the camera, return INFINITY
    if (t_old == INFINITY) {
        return INFINITY;
    }

    // Compute the point along the ray we're at at time t = t_old
    Vector3d coord = ray.At(t_old);

    // Compute the value of the inside-outside function at time t = t_old
    double g = InsideOutsideFunc(coord(0), coord(1), coord(2), e, n);

    // Compute the gradient of the inside-outside function at time t = t_old
    double dg =  ray.direction.transpose() * InsideOutsideGrad(coord(0), coord(1), coord(2), e, n);

    // If we're on the surface of the superquadric already, just return the current t (t_old);
    if (abs(dg) <= epsilon && abs(g) <= epsilon) {
        return t_old;
    }

    int iter = 0;
    // Else, solve for t_final iteratively until stopping condition, which is when both our gradient
    // and function is sufficently close to 0 or when MAX_ITERS is reached
    while (iter < MAX_ITERS) {
        // Update time step of t_new
        double t_new = t_old - g / dg;

        // Recompute the point in the ray we're at t = t_new
        coord = ray.At(t_new);

        // Recompute the gradient of the inside-outside function at time t = t_new
        dg =  ray.direction.transpose() * InsideOutsideGrad(coord(0), coord(1), coord(2), e, n);

        // Recompute the value of the inside-outside function at time t = t_new
        g = InsideOutsideFunc(coord(0), coord(1), coord(2), e, n);

        // If g'(t) flips from negative to positive, break out of loop
        if (abs(dg) <= epsilon && abs(g) <= epsilon) {
            break;
        }

        // Else, Set t_new to the t_old
        t_old = t_new;
        iter++;
    }

    return t_old;
}

pair<double, Intersection> Superquadric::ClosestIntersection(const Ray &ray) {
    // First, we want to apply the inverse transformation matrix onto our
    // ray to convert from parent-space to body-space

    // Initialize identity matrix
    Matrix4d inverse_transform = Matrix4d::Identity();

    // Compute the inverse transform used to take the ray from parent-space to
    // body-space
    for (auto it = transforms.rbegin(); it != transforms.rend(); it++) {
        // Get our inverse transform matrix
        Matrix4d transform = (*it)->GetMatrix();
        inverse_transform = transform.inverse() * inverse_transform;
    }

    // Call Transformed function to convert parent-space ray into body-space ray
    Ray ray_body = ray.Transformed(inverse_transform);

    // Use the Newton Iterative Solver to find the final t such that the inside-outside function
    // is close to 0
    double t_final = NewtonIterativeSolver(ray_body, exp0, exp1);

    // If our ray misses the object completely OR the object can't be seen by the camera,
    // return a NULL object that indicates that there is no intersection
    if (t_final == INFINITY) {
        pair<double, Intersection> closest = make_pair(INFINITY, Intersection());
        return closest;
    }

    // Find the origin - position of intersection using the ray in body space
    Vector3d origin = ray_body.At(t_final);
    
    // Find the normal vector corresponding to the body-space intersecting position
    Vector3d normal = GetNormal(origin);

    // Initialize our ray object
    Ray loc = Ray();
    loc.origin = origin;
    loc.direction = normal;

    // Initialize identity matrix for our final trasnform
    Matrix4d final_transform = Matrix4d::Identity();
    for (auto it = transforms.begin(); it != transforms.end(); it++) {
        final_transform = (*it)->GetMatrix() * final_transform;
    }

    // Transform the intersection ray from body-space to parent-space
    loc.Transform(final_transform);

    // Returns a pair indicating the timestep the intersection happens
    pair<double, Intersection> closest = make_pair(t_final, Intersection(loc, this));
    return closest;
}

pair<double, Intersection> Assembly::ClosestIntersection(const Ray &ray) {
    // First, we want to apply the inverse transformation matrix onto our
    // ray to convert from parent-space to assembly-space

    // Initialize identity matrix
    Matrix4d inverse_transform = Matrix4d::Identity();

    // Compute the inverse transform used to take the ray from parent-space to
    // assembly-space
    for (auto it = transforms.rbegin(); it != transforms.rend(); it++) {
        // Get our inverse transform matrix
        Matrix4d transform = (*it)->GetMatrix();
        inverse_transform = transform.inverse() * inverse_transform;
    }

    // Call Transformed function to convert parent-space ray into assembly-space ray
    Ray ray_assembly = ray.Transformed(inverse_transform);

    // Variable holder to keep track of the intersection closest to the assembly
    pair<double, Intersection> global_closest = make_pair(INFINITY, Intersection());
    for (auto &child : children) {
        pair<double, Intersection> child_closest = child->ClosestIntersection(ray_assembly);
        // If our child's t value is less than current global closest value, then set the
        // global closest to this child's closest intersection
        if (child_closest.first < global_closest.first) {
            global_closest = child_closest;
        }
    }

    // If the given ray intersects with none of the children, then we just return the current global
    // closest as is without transforming (since we know the ray missed the assembly)
    if (global_closest.first == INFINITY) {
        return global_closest;
    }
    
    // Else, we need to transform our closest intersection ray from assembly-space BACK to parent-space

    // Initialize identity matrix for our final trasnform
    Matrix4d final_transform = Matrix4d::Identity();

    for (auto it = transforms.begin(); it != transforms.end(); it++) {
        final_transform = (*it)->GetMatrix() * final_transform;
    }

    // Transform the intersection ray from body-space to parent-space
    global_closest.second.location.Transform(final_transform);

    return global_closest;
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
