#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <fstream>
#include "./object.h"
#include "./transform.h"
#include "./scene.h"

// This function parses an .obj file and returns the corresponding Object
Object create_object(const char* filename);

// This function parses a block of text containing the transformation matrices and returns
// the corresponding transformation object
Transformation create_transformation(ifstream &ifs);

// This function parses block sof text containing the camera information, the perspective
// projection matrix parameters, and the light sources parameters
// and returns an empty Scene with the given camera, perspective, and light source settings
// and the given resolution.
Scene create_scene(ifstream &ifs, int xres, int yres);

// This function parse a block of text containing information about the light sources
// returns a vector of Light objects with the correct parameters
vector<Light> create_lights(ifstream &ifs);

// THis function parse a block of text containing information about the material
// properties of an Object and return a Material object with the correct parameters
Material create_material(ifstream &ifs);

// This function splits the string by the delimiter into a vector<string> of tokens 
vector<string> strsplit(string &s, char delim);

#endif // #ifndef __PARSE_H__