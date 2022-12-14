#version 330 core

#define MAX_LIGHTS 8

in vec3 world_pos;
in vec3 world_norm;
in vec2 uv;

out vec4 frag_color;

uniform float ka;
uniform float kd;
uniform float ks;

uniform vec3 camera_pos;

struct Material {
    vec3 cAmbient;
    vec3 cDiffuse;
    vec3 cSpecular;
    float shininess;
    bool usesTexture;
    float repeatU;
    float repeatV;
    float blend;
};

uniform Material material;
uniform bool enableTexture;
uniform sampler2D tex;

struct Light {
    int type;
    vec3 color;
    vec3 function;
    vec3 pos;
    vec3 dir;
    float penumbra;
    float angle;
};

uniform Light lights[MAX_LIGHTS];
uniform int num_lights;

float attenuation(vec3 function, float dist) {
    return min(1 / (function[0] + dist * function[1] + dist * dist * function[2]), 1.0);
}

void main() {
    // Normalize vectors if necessary
    vec3 norm = normalize(world_norm);
    vec3 V = normalize(vec3(camera_pos) - world_pos);

    // Precompute reusable intermediate values
    vec3 kdOd = kd * material.cDiffuse;
    vec3 ksOs = ks * material.cSpecular;

    vec2 real_uv = uv;
    
    if (material.repeatU != 0 && material.repeatV != 0) {
        real_uv[0] *= material.repeatU;
        real_uv[1] *= material.repeatV;
    }

    vec4 tex_color = texture(tex, real_uv);

    frag_color = vec4(material.blend * vec3(tex_color), 1);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        // Work around constant iteration identifier
        if (i >= num_lights) break;

        // Calculate the product of attanuation and intensity
        vec3 fI = vec3(0);
        vec3 Li = vec3(0);

        // Full-length vector to light (for attenuation calculation only)
        vec3 full_Li = vec3(0);

        // Point Light
        full_Li = lights[i].pos - world_pos;
        Li = normalize(full_Li);
        fI = attenuation(lights[i].function, length(full_Li)) * lights[i].color;
        
        // Calculate the dot product
        float n_dot_L = max(min(dot(norm, Li), 1), 0);

        // Check if the normal is facing the camera
        if (n_dot_L > 0) {

            kdOd = (1 - material.blend) * vec3(tex_color);

            // Add the diffuse term
            frag_color += vec4(vec3(fI * kdOd * n_dot_L), 0);

            // Add the specular term
            vec3 Ri = -reflect(Li, norm);
            float R_dot_V = dot(Ri, V);
            R_dot_V = R_dot_V > 0 ? max(min(pow(R_dot_V, material.shininess), 1), 0) : 0;
            // frag_color += vec4(vec3(fI * ksOs * R_dot_V), 0);
        }
    }
}
