#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main()
{
    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = vec4(vec3(1.0) - vec3(texture(tex, uv)), 1.0);
}
