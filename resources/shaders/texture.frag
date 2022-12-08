#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main()
{
    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(tex, uv);
}
