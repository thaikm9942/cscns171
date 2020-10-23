#ifndef __BRESENHAM_H__
#define __BRESENHAM_H__

#include <set>
#include <tuple>

using namespace std;

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

// Generalized Bresehem's Line Algorithm that handles all slopes and octants
void generalized_bresenham(int x0, int y0, int x1, int y1, set<tuple<int, int>> &pixels);

#endif // #ifndef __BRESENHAM_H__