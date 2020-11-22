// Fragment shader
varying vec3 light_dir;

// The texture color maps and normal maps for the shader
uniform sampler2D texture_map, normal_map;

void main()
{
    // Retrieves the texture color for this pixel
    vec4 tex_color = texture2D(texture_map, gl_TexCoord[0].st);

    // Retrieves the surface normal mapping for this pixel
    vec3 n = texture2D(normal_map, gl_TexCoord[0].st).rgb;

    // Maps all components from [0, 1] -> [-1, 1] and normalizes
    n *= 2.0;
    n = normalize(n - vec3(1, 1, 1));

    // Use diffuse lighting to interpolate the color value from surface normals
    vec4 color = vec4(0, 0, 0, 0);
    float n_dot_l = max(dot(normalize(n), normalize(light_dir)), 0.0);
    if (n_dot_l > 0.0) {
        color += (gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * n_dot_l);
    }

    // Clamp the texture color to be between 0.0 and 1.0
    color = clamp(color, 0.0, 1.0);

    // The final color is the texture color multiplied by the color sum
    gl_FragColor = tex_color * color;
}