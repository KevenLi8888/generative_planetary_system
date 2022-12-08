#version 330 core

in vec2 uv;

uniform sampler2D tex;

// Equivalent "pixel" with and height
uniform float h_step;
uniform float v_step;

out vec4 fragColor;

// Emboss kernel
float kernel[9] = float[9](0.0, 1.0, 2.0, -1.0, 1.0, 1.0, -2.0, -1.0, 0.0);

void main()
{
    fragColor = vec4(0.0);

    // (u_start, v_start) is the position of the lower left corner "pixel"
    float u_start = uv[0] - h_step;
    float v_start = uv[1] - v_step;

    float u_curr = u_start;
    float v_curr = v_start;

    // Traverse the area covered by the kernel and accumulate the color values
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            fragColor += kernel[i * 3 + j] * texture(tex, vec2(u_curr, v_curr));
            u_curr += h_step;
        }
        u_curr = u_start;
        v_curr += v_step;
    }

    // Apply the coefficient to obtain the final value
    fragColor = vec4(vec3(fragColor), 1.0);
}
