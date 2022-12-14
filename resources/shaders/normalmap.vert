#version 330 core

layout(location = 0) in vec3 object_pos;
layout(location = 1) in vec3 object_norm;
layout(location = 2) in vec2 uv_in;

// Normal Mapping
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 world_pos;
out vec3 world_norm;
out vec2 uv;

// Normal Mapping
out vec3 cam_pos_tangent_space;
out vec3 pos_tangent_space;

uniform mat4 model;

// Precompute the product of proj and the view matrix
uniform mat4 proj_view;

// Precompute the inverse-transposed 3x3 model matrix
uniform mat3 model3invt;

// Normal Mapping
uniform vec3 camera_pos;

void main() {
    world_pos = vec3(model * vec4(object_pos, 1.0));
    world_norm = model3invt * object_norm;
    uv = uv_in;

    gl_Position = proj_view * vec4(world_pos, 1.0);

    // Normal Mapping
    vec3 T = normalize(model3invt * tangent);
    vec3 N = world_norm;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));
    cam_pos_tangent_space = TBN * camera_pos;
    pos_tangent_space = TBN * world_pos;
}
