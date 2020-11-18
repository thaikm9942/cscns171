// Variables passed from the vertex shader to fragment shader
varying vec3 normal, vertex;

// uniform - shared between both shaders
// attribute - only in vertex shader
// varying - from vertex shader to fragment shader

// Max number of light sources is 8
#define MAX_LIGHTS 8

// The fragment shader follows closely the lighting algorithm from Assignment 2, but is adapted to handle
// GLSL values instead
void main()
{
    // Re-normalize the normal vector just in case
    vec3 normal = normalize(normal);

    // Initialize the sum of the diffuse and specular color values for all light sources
    vec4 diffuse_sum = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 specular_sum = vec4(0.0, 0.0, 0.0, 0.0);

    // We're already in camera space with the eye at the origin of the space, so no need for an actual camera.
    // Equivalent to normalize(e - P) in the lighting algorithm
    vec3 e_dir = normalize(-vertex);

    // Iterate through each light source
    for (int i = 0; i < MAX_LIGHTS; i++) {
        // Calculate the light direction. Equivalent to normalize(l_p - P) in lighting algorithm
        vec3 l_dir = normalize(vec3(gl_LightSource[i].position) - vertex);

        // Retrieves the diffuse and specular color values of the light source
        vec4 l_diff = gl_LightSource[i].diffuse;
        vec4 l_spec = gl_LightSource[i].specular;
        vec4 l_amb = gl_LightSource[i].ambient;
        
        // Compute the distance between the point P and the light source position (in camera space)
        float dist = distance(vec3(gl_LightSource[i].position), vertex);

        // Compute the attenuation factor
        float k2 = gl_LightSource[i].quadraticAttenuation;
        float attenuation = 1.0 / (1.0 + k2 * dist * dist);

        // Attenuates the diffuse and specular lighting color values
        l_diff = l_diff * attenuation;
        l_spec = l_spec * attenuation;

        // Calculates the dot product of the normal and the light direction
        float n_dot_l = max(dot(l_dir, normal), 0.0);

        // Adds the diffuse factor of the currnet light to the current diffuse sum
        vec4 diffuse_factor = l_diff * n_dot_l;
        diffuse_sum += diffuse_factor;

        // Compute the h vector bisecting l_dir and e_dir
        vec3 h = normalize(e_dir + l_dir);

        // Calculate the dot product of the e_dir and r_dir exponentiated to the shininess
        float n_dot_h = pow(max(dot(normal, h), 0.0), gl_FrontMaterial.shininess);

        // Adds the diffuse factor of the current light to the current diffuse sum
        vec4 specular_factor = l_spec * n_dot_h;
        specular_sum += specular_factor;
    }

    // Specify the final color  
    vec4 color = gl_FrontMaterial.ambient + gl_FrontMaterial.diffuse * diffuse_sum + gl_FrontMaterial.specular * specular_sum;

    // Constrain the color value to 0.0 and 1.0
    color = clamp(color, 0.0, 1.0);
    gl_FragColor = color;
}