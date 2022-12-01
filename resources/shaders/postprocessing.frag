#version 330 core

// Task 16: Create a UV coordinate in variable
in vec3 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D postprocess_sampler;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool post_process;

uniform float width;
uniform float height;
float kernel[25];

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
//    fragColor = texture(postprocess_sampler, vec2(uv));

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
//    if (post_process) {
//        fragColor = vec4(1 - vec3(fragColor), 1);
//    }
    // Kernel Filter
    int kernel_width = 5;
    int radius = (kernel_width - 1) / 2;
    for (int i = 0; i < 25; ++i) {
        kernel[i] = 1.f/25.f;
    }

    for (int i = 0; i < 25; ++i) {
        float texture_coord_u = (uv.x * width + (i % kernel_width - radius)) / width;
        float texture_coord_v = (uv.y * height + (i / kernel_width - radius)) / height;
        fragColor += texture(postprocess_sampler, vec2(texture_coord_u, texture_coord_v)) * kernel[i];
    }
}
