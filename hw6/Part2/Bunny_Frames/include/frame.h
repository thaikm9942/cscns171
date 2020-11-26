#ifndef __FRAME_H__
#define __FRAME_H__

#include <math.h>
#include "./object.h"
#include "./utils.h"
#include <vector>

using namespace std;

/* 
 * This header file defines the Frame class representing a frame of the image in animation
 */

//////////////////////////////
///       CLASSES          ///
//////////////////////////////

// This class represents a frame with given translation, scaling and rotation components
class Frame {
    public:
        // Index of the frame
        int frame_id;

        // Boolean to indicate whether this is a keyframe
        bool is_keyframe;

        // Coordinates of the translation vector
        Object* object;

        // Default constructor
        Frame() : frame_id(0), is_keyframe(false), object(NULL) {}

        // Constructor that takes in a frame id, whether or not the frame is a keyframe, and the corresponding
        // pointer to the Object of the frame
        Frame(int id, Object* ptr, float is_keyframe) : 
            frame_id(id), object(ptr), is_keyframe(is_keyframe) {}

        // Print text representing a Frame object
        void print_frame();
};

#endif // #ifndef __FRAME_H__
