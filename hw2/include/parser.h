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

// This function parses a block of text containing the camera information and the perspective
// projection matrix parameters and returns an empty scene with camera and perspective setting
Scene create_camera_and_perspective(ifstream &ifs);

// This function splits the string by the delimiter into a vector<string> of tokens 
vector<string> strsplit(string &s, char delim);

#endif // #ifndef __PARSE_H__