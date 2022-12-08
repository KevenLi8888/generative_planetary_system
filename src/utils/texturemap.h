#pragma once

#include "utils/scenedata.h"

#include <glm/glm.hpp>

#include "glm/ext/scalar_constants.hpp"

// Global constant values
namespace Constants {
    constexpr float PI = glm::pi<float>();
    constexpr float BOUNDARY = 0.49999999;
    constexpr float EPSILON = 0.0001;
}

class TextureMap {
public:
    static glm::vec2 getUVAt(glm::vec3 p, PrimitiveType type) {
        float u = 0, v = 0, theta = 0.5, phi;

        switch (type) {
            case PrimitiveType::PRIMITIVE_SPHERE:
                if (p.x != 0 && p.z != 0) {
                    theta = atan2(p.z, p.x);
                }
                u = theta < 0 ? -theta / (2 * Constants::PI) : 1 - theta / (2 * Constants::PI);
                v = (glm::asin(p.y * 2)) / Constants::PI + 0.5;
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                if (p.y <= -Constants::BOUNDARY) {
                    u = p.x - 0.5;
                    v = p.z - 0.5;
                } else {
                    if (p.x != 0 && p.z != 0)
                        theta = atan2(p.z, p.x);
                    u = theta < 0 ? -theta / (2 * Constants::PI) : 1 - theta / (2 * Constants::PI);
                    v = p.y + 0.5;
                }
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                if (p.y <= -Constants::BOUNDARY) {
                    u = p.x - 0.5;
                    v = p.z - 0.5;
                } else if (p.y >= Constants::BOUNDARY) {
                    u = p.x - 0.5;
                    v = -p.z - 0.5;
                } else {
                    if (p.x != 0 && p.z != 0)
                        theta = atan2(p.z, p.x);
                    u = theta < 0 ? -theta / (2 * Constants::PI) : 1 - theta / (2 * Constants::PI);
                    v = p.y + 0.5;
                }
                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                if (p.x <= -Constants::BOUNDARY) {
                    u = p.z - 0.5;
                    v = p.y - 0.5;
                } else if (p.x >= Constants::BOUNDARY) {
                    u = -p.z - 0.5;
                    v = p.y - 0.5;
                } else if (p.y <= -Constants::BOUNDARY) {
                    u = p.x - 0.5;
                    v = p.z - 0.5;
                } else if (p.y >= Constants::BOUNDARY) {
                    u = p.x - 0.5;
                    v = -p.z - 0.5;
                } else if (p.z <= -Constants::BOUNDARY) {
                    u = -p.x - 0.5;
                    v = p.y - 0.5;
                } else {
                    u = p.x - 0.5;
                    v = p.y - 0.5;
                }
                break;
            default:
                // Type not yet supported
                break;
        }

        return glm::vec2(u, v);
    }
};
