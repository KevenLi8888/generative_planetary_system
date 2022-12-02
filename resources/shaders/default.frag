#version 330 core

in vec3 pos_world;
in vec3 n_world;
in vec3 uv;

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
    int type; // 0 - directional, 1 - point, 2 - spot, -1 - undefined
    vec4 color;
    vec3 function;
    vec4 dir;
    vec4 pos;
    float penumbra;
    float angle;
};

uniform GlobalData global;
uniform ShapeData shape;
uniform LightData lights[8];
uniform vec4 cam_pos_world;

// Texture Mapping
uniform sampler2D texture_sampler;
uniform float blend;
uniform float hor_repeat;
uniform float vert_repeat;

float Falloff(float x, float inner, float outer) {
    float falloff = -2 * pow((x-inner)/(outer-inner), 3.f) + 3 * pow((x-inner)/(outer-inner), 2.f);
    return falloff;
}

void main() {
    fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    vec4 normalized_n_world = vec4(normalize(n_world), 0.f);
    float texture_u = uv.x * hor_repeat;
    float texture_v = uv.y * vert_repeat;
    vec4 texture_color = texture(texture_sampler, vec2(texture_u, texture_v));

    fragColor += global.ka * shape.cAmbient;
    for (int i = 0; i < 8; ++i) {
        LightData light = lights[i];
        if (light.type == -1) { // undefined lights
            continue;
        }
        else if (light.type == 0) { // directional lights
            vec4 li = normalize(-light.dir);
            if (dot(normalized_n_world, li) > 0) {
                fragColor += light.color * (global.kd * shape.cDiffuse * (1-blend) + texture_color * blend) * dot(normalized_n_world, li);
            }
            vec4 ri = reflect(-li, normalized_n_world);
            vec4 v = normalize(cam_pos_world - vec4(pos_world, 1.f)); // direction to camera
            if (dot(ri, v) > 0) {
                fragColor += light.color * global.ks * shape.cSpecular * pow(dot(ri, v), shape.shininess);
            }
        }
        else if (light.type == 1) { // point lights
            float distance = distance(vec4(pos_world, 1.f), light.pos);
            float fatt = min(1.f, 1.f / (light.function.x + distance*light.function.y + pow(distance, 2.f)*light.function.z));
            vec4 li = normalize(light.pos - vec4(pos_world, 1.f));
            if (dot(normalized_n_world, li) > 0) {
                fragColor += fatt * light.color * (global.kd * shape.cDiffuse * (1-blend) + texture_color * blend) * dot(normalized_n_world, li);
            }
            vec4 ri = reflect(-li, normalized_n_world);
            vec4 v = normalize(cam_pos_world - vec4(pos_world, 1.f)); // direction to camera
            if (dot(ri, v) > 0) {
                fragColor += fatt * light.color * global.ks * shape.cSpecular * pow(dot(ri, v), shape.shininess);
            }
        }
        else if (light.type == 2) { // spot lights
            float distance = distance(vec4(pos_world, 1.f), light.pos);
            float fatt = min(1.f, 1.f / (light.function.x + distance*light.function.y + pow(distance, 2.f)*light.function.z));
            float outer = light.angle;
            float inner = outer - light.penumbra;
            vec4 ray_dir = vec4(pos_world, 1.f) - light.pos;
            float angle = acos(dot(light.dir, ray_dir) / (length(light.dir) * length(ray_dir)));
            vec4 li = normalize(light.pos - vec4(pos_world, 1.f));
            if (dot(normalized_n_world, li) > 0 && angle <= inner) {
                fragColor += fatt * light.color * (global.kd * shape.cDiffuse * (1-blend) + texture_color * blend) * dot(normalized_n_world, li);
            }
            else if (dot(normalized_n_world, li) > 0 && angle > inner && angle <= outer) {
                fragColor += (1 - Falloff(angle, inner, outer)) * fatt * light.color * (global.kd * shape.cDiffuse * (1-blend) + texture_color * blend) * dot(normalized_n_world, li);
            }
            vec4 ri = reflect(-li, normalized_n_world);
            vec4 v = normalize(cam_pos_world - vec4(pos_world, 1.f)); // direction to camera
            if (dot(ri, v) > 0) {
                fragColor += fatt * light.color * global.ks * shape.cSpecular * pow(dot(ri, v), shape.shininess);
            }
        }
    }
    fragColor = min(max(fragColor, 0.f), 1.f);
//    fragColor = vec4(uv, 1);
}
