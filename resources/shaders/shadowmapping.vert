#version 330 core
layout(location = 0) in vec3 pos_object_1;

uniform mat4 light_space_matrix; // light proj * light view
uniform mat4 model_mat_1;

void main() {
    gl_Position = light_space_matrix * model_mat_1 * vec4(pos_object_1, 1.0);
}