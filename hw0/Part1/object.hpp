#ifndef __OBJECT_H__
#define __OBJECT_H__

// This struct represents a vertex with (x, y, z) float coordinates 
struct Vertex {
    float x;
    float y;
    float z;
};

/* This struct represents a face containing 3 integers representing 
 * the 3 1-indexed vertices forming that face.
 */ 
struct Face {
    int i1;
    int i2;
    int i3;
};

/* This struct represents a graphics object with various
 * definitions depending on the program */
struct Object;


#endif // #ifndef __PARSE_H__
