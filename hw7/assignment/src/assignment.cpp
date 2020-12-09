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

    // Solve the quadratic equations for our function
    double t1 = sign(b) * (-abs(b) - sqrt(delta)) / (2.0 * a);
    double t2 = sign(b) * (2.0 * c) / (-abs(b) - sqrt(delta));

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
    double epsilon = 1e-3;

    // Solve for the initial guess of t using quadratic equations
    double t_old = GetInitialGuess(ray);

    // If our guess is negative infinity, then this ray misses the superquadric completely or
    // our object is behind the camera, return INFINITY
    if (t_old == INFINITY) {
        return INFINITY;
    }

    // Compute the point along the ray we're at time t = t_old
    Vector3d coord = ray.At(t_old);

    // Compute the value of the inside-outside function at time t = t_old
    double g = InsideOutsideFunc(coord(0), coord(1), coord(2), e, n);

    // Compute the gradient of the inside-outside function at time t = t_old
    double dg =  ray.direction.transpose() * InsideOutsideGrad(coord(0), coord(1), coord(2), e, n);

    // If both g(t) and g'(t) are sufficiently small enough, just return t_old
    if (abs(dg) <= epsilon && abs(g) <= epsilon) {
        return t_old;
    }

    int iter = 0;
    // Else, solve for t_final iteratively until stopping condition, which is when both our gradient
    // and function is sufficently close to 0 or when MAX_ITERS is reached
    while (iter < MAX_ITERS) {
        // If the gradient switches to positive or our g(t) function is small enough,
        // then we stop
        if ((dg > 0 && g > 0) || abs(g) <= epsilon) {
            break;
        }

        // Update time step of t_new
        double t_new = t_old - g / dg;

        // Recompute the point in the ray we're at t = t_new
        coord = ray.At(t_new);

        // Recompute the gradient of the inside-outside function at time t = t_new
        dg = ray.direction.transpose() * InsideOutsideGrad(coord(0), coord(1), coord(2), e, n);

        // Recompute the value of the inside-outside function at time t = t_new
        g = InsideOutsideFunc(coord(0), coord(1), coord(2), e, n);

        // Set t_new to the t_old
        t_old = t_new;

        // Increment iteration
        iter++;
    }

    // Might be possible that when we terminate, both g and dg be positive
    if (abs(g) > epsilon) {
        return INFINITY;
    }

    // If we haven't stopped after MAX_ITERS iteration, then just return the current t_old
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

    // Create homogenous version of coordinate
    Vector4d h_origin = Vector4d(origin(0), origin(1), origin(2), 1.0);

    // Initialize identity matrix for our final trasnform
    Matrix4d final_transform = Matrix4d::Identity();
    for (auto it = transforms.begin(); it != transforms.end(); it++) {
        final_transform = (*it)->GetMatrix() * final_transform;
    }

    // Apply transformation to transform origin from body-space to parent-space
    h_origin = final_transform * h_origin;
    h_origin = h_origin / h_origin(3);

    // Apply inverse transpose of the 3x3 part of the transform to the surface normal and normalize the
    // distance and the normal
    normal = final_transform.block<3, 3>(0, 0).inverse().transpose() * normal;
    t_final = t_final / normal.norm();
    normal.normalize();

    // Initialize our transformed ray object
    Ray loc = Ray();
    loc.origin = h_origin.head(3);
    loc.direction = normal;

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

    // Transform the intersection ray from assembly-space to parent-space
    Ray intersection = global_closest.second.location;
    Vector3d origin = intersection.origin;
    Vector3d normal = intersection.direction;

    // Create homogenous version of coordinate
    Vector4d h_origin = Vector4d(origin(0), origin(1), origin(2), 1.0);
    
    // Apply transformation to transform origin from body-space to parent-space and divide
    // by the w-component
    h_origin = final_transform * h_origin;
    h_origin = h_origin / h_origin(3);

    // Apply inverse transpose of the 3x3 part of the transform to the surface normal and normalize it
    normal = final_transform.block<3, 3>(0, 0).inverse().transpose() * normal;
    global_closest.first = global_closest.first / normal.norm();
    normal.normalize();

    // Initialize our transformed ray object
    Ray loc = Ray();
    loc.origin = h_origin.head(3);
    loc.direction = normal;
    
    // Set the ray of the global closest to be the new transformed ray
    global_closest.second.location = loc;
    
    return global_closest;
}


/////////////////////////////////
////     PART 2 FUNCTIONS    ////
/////////////////////////////////

/* Function that computes the lighting model for Phong shading
 * @param p - the vertex (in this case, our first intersection point)
 * @param n - surface normal at that vertex
 * @param mat - the material of the object our vertex intersected with
 * @param lights - the light sources
 * @param e - position of camera
 */
Vector3f Lighting(Vector3d &p, Vector3d &n, Material &mat, vector<Light> &lights, Vector3d &e) {
    // Finds the direction from our vertex p to our camera and normalizes it
    Vector3d e_dir = e - p;
    e_dir.normalize();

    // Initialize the sum of diffusion reflectance and specular reflectance
    Vector3d diff_sum(0.0, 0.0, 0.0);
    Vector3d spec_sum(0.0, 0.0, 0.0);

    for (auto &light : lights) {
        // Retrieves the light position and color
        Vector4d l = light.position;
        Vector3d color(light.color.r, light.color.g, light.color.b);

        // Find the vector pointing from our vertex to the light source and normalize it
        Vector3d l_dir = l.head(3) - p;

        // Computes the distance between the light source and the vertex
        double d = l_dir.norm();

        // Attenuates the color using the given attenuation constant (linear attenuation)
        color *= 1.0 / (1.0 + d * light.attenuation);

        // Normalize our l_dir
        l_dir.normalize();

        // Computes the dot product between the n and the l_dir
        double nl_dot = n.transpose() * l_dir;

        // Computes the diffusion factor to add our color to the diffusion sum
        double diff_factor = max(0.0, nl_dot);
        diff_sum += diff_factor * color;

        // Obtain the h vector bisecting l_dir and e_dir and normalize it
        Vector3d h = e_dir + l_dir;
        h.normalize();

        // Computes the dot product between n and h
        double nh_dot = n.transpose() * h;

        // Computes the specular factor, which is the (positive) dot product to the power of the shininess
        double spec_factor = pow(max(0.0, nh_dot), mat.shininess);
        spec_sum += spec_factor * color;
    }

    Vector3f cp;
    cp(0) = min(1.0, mat.ambient.r + diff_sum(0) * mat.diffuse.r + spec_sum(0) * mat.specular.r);
    cp(1) = min(1.0, mat.ambient.g + diff_sum(1) * mat.diffuse.g + spec_sum(1) * mat.specular.g);
    cp(2) = min(1.0, mat.ambient.b + diff_sum(2) * mat.diffuse.b + spec_sum(2) * mat.specular.b);
    
    return cp;
}


/**
 * Raytracing Code
 */

void Scene::Raytrace() {
    // Initialize image of size XRES x YRES
    Image img = Image(XRES, YRES);

    // Get the camera from the scene
    Camera camera = GetCamera();

    // The camera position is the opposite of the translate deltas in the Camera object
    Vector3d cam_pos = -camera.translate.GetDelta();

    // Get the light sources from the scene
    vector<Light> lights = GetLights();

    // Retrieves the camera frustum from the Scene
    Frustum frust = camera.frustum;

    // Need to retrieve the height and width of the front plane of the frustum
    double h = 2.0 * frust.near * tan((frust.fov * M_PI / 180) / 2.0);
    double a = (double) XRES / YRES; // Aspect ratio is the x-resolution divided by y-resolution
    double w = h * a; // Width is just the height * aspect ratio

    // Initialize basis vectors
    Vector3d e1(0, 0, -1); // Direction where the camera is looking
    Vector3d e2(1, 0, 0); // Vector pointing directly to the right relative to the camera
    Vector3d e3(0, 1, 0); // Vector pointing upwards relative to camera

    // For each pixel (i, j) in the image, want to send out rays through each pixel from our camera
    for (int i = 0; i < XRES; i++) {
        for (int j = 0; j < YRES; j++) {
            double x = (i - XRES / 2.0) * w / (double) XRES; // Obtain the x-coordinate of the pixel (i, j)
            double y = (j - YRES / 2.0) * h / (double) YRES; // Obtain the y-coordinate of the pixel (i, j)

            // Computes the direction of the ray going from the camera to each pixel on the grid
            Vector3d direction = frust.near * e1 + x * e2 + y * e3;

            // Create the ray to send out from the camera position to each pixel on the grid
            Ray incoming = {cam_pos, direction};

            // Finds the closest intersection from each pixel to the screen plane
            pair<double, Intersection> closest = ClosestIntersection(incoming);

            // If there is no closest intersection (i.e, the ray misses the screen plane completely),
            // color it black
            if (closest.first == INFINITY) {
                img.SetPixel(i, j, Vector3f::Zero());
                continue;
            }

            // Get the intersection object
            Superquadric* object = closest.second.obj;

            // Get the intersection ray
            Ray intersection = closest.second.location;
            
            // Get the point of intersection
            Vector3d point = intersection.origin;

            // Get the surface normal at that point
            Vector3d normal = intersection.direction;

            // Get the material of the intersecting object
            Material mat = object->GetMaterial();

            // Compute the color of this pixel using the lighting model
            Vector3f color = Lighting(point, normal, mat, lights, cam_pos);

            // The pixel to the correct color
            img.SetPixel(i, j, color);
        }
    }

    // Outputs the image.
    if (!img.SaveImage("rt.png")) {
        cerr << "Error: couldn't save PNG image" << std::endl;
    } else {
        cout << "Done!\n";
    }
}
