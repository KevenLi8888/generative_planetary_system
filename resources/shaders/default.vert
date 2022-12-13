#version 330 core

layout(location = 0) in vec3 pos_object;
layout(location = 1) in vec3 n_object;
layout(location = 2) in vec3 uv_object;

// Normal Mapping
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 pos_world;
out vec3 n_world;
out vec3 uv;
out vec4 pos_light_space[8]; // all lights (only relevent for directional lights)

// Normal Mapping
out vec4 light_dir_tangent_space[8];
out vec4 light_pos_tangent_space[8];
out vec4 cam_pos_tangent_space;
out vec3 pos_tangent_space;

uniform mat4 model_mat;
uniform mat3 inversed_transposed_model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 light_space_mat[8]; // all lights (only relevent for directional lights)

// Normal Mapping
uniform vec4 cam_pos_world;
uniform vec4 light_dir[8];
uniform vec4 light_pos[8];

uniform bool enable_terrain_generation;


void main() {
    pos_world = vec3(model_mat * vec4(pos_object, 1));
    n_world = normalize(inversed_transposed_model_mat * n_object);
    uv = uv_object;
    gl_Position = proj_mat * view_mat * vec4(pos_world, 1);
    for (int i = 0; i < 8; ++i) {
        pos_light_space[i] = light_space_mat[i] * vec4(pos_world, 1);
    }

    // Normal Mapping
    vec3 T = normalize(inversed_transposed_model_mat * tangent);
//    vec3 T = normalize(vec3(model_mat * vec4(tangent, 0)));
    vec3 N = n_world;
//    vec3 N = normalize(vec3(model_mat * vec4(n_object, 0)));
//    vec3 B = normalize(inversed_transposed_model_mat * bitangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));
//    mat3 TBN = mat3(T, B, N);
    cam_pos_tangent_space = vec4(TBN * vec3(cam_pos_world), 1.0);
    pos_tangent_space = TBN * pos_world;
    for (int i = 0; i < 8; ++i) {
        light_dir_tangent_space[i] = vec4(TBN * vec3(light_dir[i]), 1.0);
        light_pos_tangent_space[i] = vec4(TBN * vec3(light_pos[i]), 1.0);
    }

    // camera space
////    vec3 T = normalize(vec3(view_mat * vec4(inversed_transposed_model_mat * tangent, 0)));
//    vec3 T = normalize(vec3(view_mat * model_mat * vec4(tangent, 0)));
//    //    vec3 T = normalize(vec3(model_mat * vec4(tangent, 0)));
//    vec3 N = normalize(vec3(view_mat * model_mat * vec4(n_object, 0)));
//    //    vec3 B = normalize(inversed_transposed_model_mat * bitangent);
////    T = normalize(T - dot(T, N) * N);
////    vec3 B = cross(N, T);
//    vec3 B = normalize(vec3(view_mat * model_mat * vec4(bitangent, 0)));
//    mat3 TBN = transpose(mat3(T, B, N));
//    cam_pos_tangent_space = vec4(TBN * vec3(0, 0, 0), 1.0);
//    pos_tangent_space = TBN * vec3(view_mat * vec4(pos_world, 1));
//    for (int i = 0; i < 8; ++i) {
//        light_dir_tangent_space[i] = vec4(TBN * vec3(view_mat * light_dir[i]), 1.0);
//        light_pos_tangent_space[i] = vec4(TBN * vec3(view_mat * light_pos[i]), 1.0);
//    }
}
