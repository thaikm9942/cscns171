#include "../include/frame.h"
#include <stdio.h>

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////

void Frame::print_frame() {
    printf("Frame %d\n", frame_id);
    printf("translation: %f %f %f\n", t[0], t[1], t[2]);
    printf("scaling: %f %f %f\n", s[0], s[1], s[2]);
    printf("rotation: %f %f %f %f\n", r[0], r[1], r[2], angle);
    printf("quarternion:\n");
    print_quaternion(q);
    printf("\n");
}