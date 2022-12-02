#version 330 core

layout(location = 0) in vec3 pos_object;
layout(location = 1) in vec3 n_object;
layout(location = 2) in vec3 uv_object;

out vec3 pos_world;
out vec3 n_world;
out vec3 uv;

uniform mat4 model_mat;
uniform mat3 inversed_transposed_model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main() {
    pos_world = vec3(model_mat * vec4(pos_object, 1));
    n_world = normalize(inversed_transposed_model_mat * n_object);
    uv = uv_object;
    gl_Position = proj_mat * view_mat * vec4(pos_world, 1);
}
