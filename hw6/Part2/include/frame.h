#ifndef __FRAME_H__
#define __FRAME_H__

#include <math.h>
#include "./quaternion.h"
#include "./utils.h"

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
        float t[3]; 

        // Coordinates of the scaling vector
        float s[3];

        // Coordinates of the rotation axis
        float r[3];
        
        // Rotation angle in degrees
        float angle;

        // Quaternion representation of the rotation matrix
        Quaternion q;

        // Default constructor
        Frame() : frame_id(0), angle(0.0), is_keyframe(false) {
            t[0] = 0.0, t[1] = 0.0, t[2] = 0.0;
            s[0] = 0.0, s[1] = 0.0, s[2] = 0.0;
            r[0] = 0.0, r[1] = 0.0, r[2] = 0.0;
            q = rot2quar(r, 0.0);
        }

        // Constructor that takes in a given translation, scaling and rotation components for each frame as well
        // as a boolean indicating whether or not the frame is a keyframe. Automatically compute the quaternion
        // from the given rotation vector
        Frame(int id, float tn[3], float sn[3], float rn[3], float theta, float is_keyframe) : 
            frame_id(id), angle(theta), is_keyframe(is_keyframe) {
            t[0] = tn[0], t[1] = tn[1], t[2] = tn[2];
            s[0] = sn[0], s[1] = sn[1], s[2] = sn[2];
            r[0] = rn[0], r[1] = rn[1], r[2] = rn[2];
            
            // Normalize the rotation vector to get a unit vector
            normalize(rn);
            // Converts the angle from degrees to radians since our quaternion implementation
            // calculates the entries based on radians
            q = rot2quar(rn, deg2rad(theta));
        }

        Frame(int id, float tn[3], float sn[3], float rn[3], float theta, Quaternion qn, float is_keyframe) : 
            frame_id(id), angle(theta), is_keyframe(is_keyframe) {
            t[0] = tn[0], t[1] = tn[1], t[2] = tn[2];
            s[0] = sn[0], s[1] = sn[1], s[2] = sn[2];
            r[0] = rn[0], r[1] = rn[1], r[2] = rn[2];
            q = qn;
        }

        // Print text representing a Frame object
        void print_frame();
};

//////////////////////////////
///       FUNCTIONS        ///
//////////////////////////////

#endif // #ifndef __FRAME_H__
