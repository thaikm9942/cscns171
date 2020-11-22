// Vertex shader
varying vec3 light_dir;

// The texture color maps and normal maps for the shader
uniform sampler2D texture_map, normal_map;

void main()
{
    // Set the per-vertex texture coordinate to be interpolated in the fragment shader
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // Retrieves the surface normal mapping for this pixel
    vec3 n = texture2D(normal_map, gl_TexCoord[0].st).rgb;

    // Maps all components from [0, 1] -> [-1, 1] and normalizes
    n *= 2.0;
    n = normalize(n - vec3(1, 1, 1));

    // Tangent is always (-1.0, 0.0, 0.0) considering our normal vector points in the z-axis out of an xy plane
    vec3 t = vec3(-1.0, 0.0, 0.0);

    // Binormal is normal cross tangent
    vec3 b = cross(n, t);

    // Convert the camera and light sources from world space to camera space
    vec3 view_pos = vec3(gl_ModelViewMatrix * gl_Vertex);
    vec3 light_pos = vec3(gl_LightSource[0].position);

    // Converts the camera and light sources from camera space to surface space
    view_pos = vec3(dot(t, view_pos), dot(b, view_pos), dot(n, view_pos));
    light_pos = vec3(dot(t, light_pos), dot(b, light_pos), dot(n, light_pos));

    // Calculate the light direction in surface coordinates
    light_dir = normalize(light_pos - view_pos);

    // Convert vertex from world space to NDC
    gl_Position = ftransform();
}