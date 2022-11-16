#version 330 core

in vec3 pos_world;
in vec3 n_world;

out vec4 fragColor;

struct GlobalData {
    float ka; // Ambient term
    float kd; // Diffuse term
    float ks; // Specular term
};

struct ShapeData {
    vec4 cAmbient;     // Ambient term
    vec4 cDiffuse;     // Diffuse term
    vec4 cSpecular;    // Specular term
    float shininess;         // Specular exponent
};

struct LightData {
    // Only supports directional light
    vec4 color;
    vec4 dir;
};

uniform GlobalData global;
uniform ShapeData shape;
uniform LightData lights[8];
uniform vec4 cam_pos_world;

void main() {
    fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    vec4 normalized_n_world = vec4(normalize(n_world), 0.f);

    fragColor += global.ka * shape.cAmbient;
    for (int i = 0; i < 8; ++i) {
        LightData light = lights[i];
        if (light.color == vec4(0.f, 0.f, 0.f, 0.f)) {
            continue;
        }
        vec4 li = normalize(-light.dir);
        if (dot(normalized_n_world, li) > 0) {
            // fragColor += light.color * global.kd * shape.cDiffuse * min(max(dot(normalized_n_world, li), 0.f), 1.f);
            fragColor += light.color * global.kd * shape.cDiffuse * dot(normalized_n_world, li);
        }
        vec4 ri = reflect(-li, normalized_n_world);
        vec4 v = normalize(cam_pos_world - vec4(pos_world, 1.f)); // direction to camera
        if (dot(ri, v) > 0) {
            // fragColor += light.color * global.ks * shape.cSpecular * pow(min(max(dot(ri, v), 0.f), 1.f), shape.shininess);
            fragColor += light.color * global.ks * shape.cSpecular * pow(dot(ri, v), shape.shininess);
        }
    }
    fragColor = min(max(fragColor, 0.f), 1.f);
}
