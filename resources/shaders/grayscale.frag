#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main()
{
    // Uisng the Luma method to convert color into grayscale
    vec4 color = texture(tex, uv);
    float gray = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    
    fragColor = vec4(vec3(gray), 1.0);
}
