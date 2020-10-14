#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "./object.h"
#include "./transform.h"

// This function parses an .obj file from the filestream and returns the corresponding object
Object get_object(ifstream &ifs);

// This function parses a block of text containing the transformation details and returns
// the corresponding transformations
Transformation create_transformation(ifstream &ifs);

#endif // #ifndef __PARSE_H__