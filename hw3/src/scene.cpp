#include "../include/scene.h"

//////////////////////////////
///    CLASS FUNCTIONS     ///
//////////////////////////////
void Perspective::print_perspective() {
    printf("perspective:\n");
    printf("n: %f, f: %f, l: %f, r: %f, t: %f, b: %f\n", near, far, left, right, top, bottom);
    printf("\n");
}


void Camera::print_camera() {
    printf("camera:\n");
    printf("position: %f %f %f\n", position[0], position[1], position[2]);
    printf("orientation: %f %f %f %f\n", orientation[0], orientation[1], orientation[2], angle);
    printf("\n");
}

void Scene::print_scene() {
    camera.print_camera();
    perspective.print_perspective();
    for (size_t i = 0; i < objects.size(); i++) {
        printf("%s\n", objects[i].label.c_str());
        objects[i].obj.print_object();
    }
    printf("\n");
    for (int i = 0; i < light_sources.size(); i++) {
        printf("light%d\n", i);
        light_sources[i].print_light();
    }
}

void Scene::add_labeled_object(Labeled_Object l_obj) {
    objects.push_back(l_obj);
}

void Scene::add_labeled_object(Object obj, string label) {
    Labeled_Object l_obj = {label, obj};
    add_labeled_object(l_obj);
}