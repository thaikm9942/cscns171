#include "../include/halfedge.h"

Mesh_Data* create_mesh_data(Object obj) {
    Mesh_Data* mesh_data = new Mesh_Data();
    vector<Vertex*> *vertex_pointers = new vector<Vertex*>;
    vector<Face*> *face_pointers = new vector<Face*>;

    // Add a NULL pointer at the start of the vector of vertex pointers due to 1-indexing
    vertex_pointers->push_back(NULL);

    // Add the pointer to the vertex to the vector of vertex pointers
    for (int i = 1; i < obj.vertices.size(); i++) {
        vertex_pointers->push_back(new Vertex(obj.vertices[i]));
    }

    // Add the pointer to the face to the vector of face pointers
    for (int i = 0; i < obj.faces.size(); i++) {
        face_pointers->push_back(new Face(obj.faces[i]));
    }

    // Set the pointer to the vector of vertex pointers and vector of face pointers
    mesh_data->vertices = vertex_pointers;
    mesh_data->faces = face_pointers;
    
    return mesh_data;
}

void compute_vertex_normal(HEV* hev) {
    // Obtain the half-edge going out of this vertex
    HE* he = hev->out;

    // Initialize our vertex normal for this vertex
    Vertex vertex_normal = Vertex(0.0, 0.0, 0.0);

    // For every vertex adjacent to this halfedge vertex, add the weighted normal of its incident face
    // onto this normal
    do {
        // Compute the face normal and face area
        Vertex normal = compute_face_normal(he->face);
        float area = compute_face_area(he->face);

        // Adds the weighted normal to the current weighted normal sum
        vertex_normal = add(vertex_normal, dot(area, normal));

        // Move onto the next adjacent vertex (using its halfedge)
        he = he->flip->next;
    }
    // Repeat this until we've reached the first adjacent vertex
    while (he != hev->out);

    // Normalizes the vertex normal
    normalize(vertex_normal);

    // Sets the current normal of the current halfedge vertex to be the obtained normal
    hev->normal = vertex_normal;
}

Vertex compute_face_normal(HEF* face) {
    // Retrieves the 3 vertices that form this face
    HEV* v1 = face->edge->vertex;
    HEV* v2 = face->edge->next->vertex;
    HEV* v3 = face->edge->next->next->vertex;

    // The face normal is the cross product of (v2 - v1) and (v3 - v1)
    Vertex a = Vertex(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z);
    Vertex b = Vertex(v3->x - v1->x, v3->y - v1->y, v3->z - v1->z);
    Vertex normal = cross(a, b);
    return normal;
}

float compute_face_area(HEF* face) {
    Vertex n = compute_face_normal(face);
    // The face area is the half of the Euclidean norm of the face normal
    return norm(n) / 2.0;
}

void print_mesh_data(Mesh_Data* mesh_data) {
    vector<Vertex*> *vertices = mesh_data->vertices;
    vector<Face*> *faces = mesh_data->faces;
    printf("Mesh Data:\n");
    // Print each vertex
    for (int i = 1; i < vertices->size(); i++) {
        vertices->at(i)->print_vertex();
    }

    // Print each face
    for (int i = 0; i < faces->size(); i++) {
        faces->at(i)->print_face();
    }
}