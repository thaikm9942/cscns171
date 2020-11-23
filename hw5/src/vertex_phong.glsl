// Variables to be passed into the fragment shader
varying vec3 normal, vertex;

// Simple vertex shader for the Phong shading algorithm
void main()
{
    // Converts the normal vertex and the vertex position from object space to world
    normal = normalize(gl_NormalMatrix * gl_Normal);
    vertex = vec3(gl_ModelViewMatrix * gl_Vertex);

    // Applies the perspective transformation to convert from world space to NDC
    gl_Position = ftransform();
}