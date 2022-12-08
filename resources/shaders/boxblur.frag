#version 330 core

in vec2 uv;

uniform sampler2D tex;

// Equivalent "pixel" with and height
uniform float h_step;
uniform float v_step;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.0);

    // (u_start, v_start) is the position of the lower left corner "pixel"
    float u_start = uv[0] - h_step * 2;
    float v_start = uv[1] - v_step * 2;

    float u_curr = u_start;
    float v_curr = v_start;

    // Traverse the area covered by the kernel and accumulate the color values
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            fragColor += texture(tex, vec2(u_curr, v_curr));
            u_curr += h_step;
        }
        u_curr = u_start;
        v_curr += v_step;
    }

    // Apply the coefficient to obtain the final value
    fragColor = vec4(vec3(fragColor)/25.0, 1.0);
}
