#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

// Lambda values for each channel
float e1 = 0.2;
float e2 = 0.1;
float e3 = 0.05;

// UV coordinate of the center pixel
vec2 center = vec2(0.5, 0.5);

// Backmapping with pincushion distortion
vec2 backmapping(vec2 xy, float dist, float lambda) {
    float scalar = 1.0 + lambda * pow(dist, 2);
    return xy / scalar;
}

void main()
{
    // Shift the origin from (0, 0) to the center (0.5, 0.5)
    vec2 xy = uv - center;
    float dist = length(xy);

    // Obtain the color for the red, green, blue channels separately
    float r = texture(tex, center + backmapping(xy, dist, e1)).r;
    float g = texture(tex, center + backmapping(xy, dist, e2)).g;
    float b = texture(tex, center + backmapping(xy, dist, e3)).b;

    // Combine all channels to obtain the final color
    fragColor = vec4(r, g, b, 1.0);
}
