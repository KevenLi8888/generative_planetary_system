#version 330 core

// Task 16: Create a UV coordinate in variable
in vec3 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D postprocess_sampler;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool grayscale;
uniform bool invert;
uniform bool blur;
uniform bool sharpen;

uniform float width;
uniform float height;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(postprocess_sampler, vec2(uv));

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    // Per-pixel filter
    if (grayscale) {
        float gray = 0.299 * fragColor.r + 0.587 * fragColor.g + 0.114 * fragColor.b;
        fragColor = vec4(gray, gray, gray, 1);
    }
    if (invert) {
        fragColor = vec4(1 - vec3(fragColor), 1);
    }

    // Kernel Filter
    if (blur) {
        fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        float blur[25];
        int kernel_width = 5;
        int radius = (kernel_width - 1) / 2;
        for (int i = 0; i < 25; ++i) {
            blur[i] = 1.f/25.f;
        }

        for (int i = 0; i < 25; ++i) {
            float texture_coord_u = (uv.x * width + (i % kernel_width - radius)) / width;
            float texture_coord_v = (uv.y * height + (i / kernel_width - radius)) / height;
            fragColor += texture(postprocess_sampler, vec2(texture_coord_u, texture_coord_v)) * blur[i];
        }
    }
    if (sharpen) {
        fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        float sharpen[9];
        int kernel_width = 3;
        int radius = (kernel_width - 1) / 2;
        for (int i = 0; i < 9; ++i) {
            sharpen[i] = 1.f/9.f * -1;
        }
        sharpen[4] = 1.f/9.f * 17;

        for (int i = 0; i < 9; ++i) {
            float texture_coord_u = (uv.x * width + (i % kernel_width - radius)) / width;
            float texture_coord_v = (uv.y * height + (i / kernel_width - radius)) / height;
            fragColor += texture(postprocess_sampler, vec2(texture_coord_u, texture_coord_v)) * sharpen[i];
        }
    }
}
