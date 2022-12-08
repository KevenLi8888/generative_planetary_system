#version 330 core

layout(location = 0) in vec3 object_pos;
layout(location = 1) in vec3 object_norm;
layout(location = 2) in vec2 uv_in;

out vec3 world_pos;
out vec3 world_norm;
out vec2 uv;

uniform mat4 model;

// Precompute the product of proj and the view matrix
uniform mat4 proj_view;

// Precompute the inverse-transposed 3x3 model matrix
uniform mat3 model3invt;

void main() {
    world_pos = vec3(model * vec4(object_pos, 1.0));
    world_norm = model3invt * object_norm;
    uv = uv_in;
    
    gl_Position = proj_view * vec4(world_pos, 1.0);
}
