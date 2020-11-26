#include "../include/frame.h"
#include <stdio.h>

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////

void Frame::print_frame() {
    if (is_keyframe) {
        printf("Keyframe %d\n", frame_id);
    }
    else {
        printf("Frame %d\n", frame_id);
    }
    // Prints the object associated with this frame
    object->print_object();
}