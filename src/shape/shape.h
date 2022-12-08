#pragma once

#include <glm/glm.hpp>
#include <vector>

class Shape
{
public:
    static std::vector<float> generateShape(int param1, int param2);

protected:
    inline static std::vector<float> m_vertexData;

    static void insertVec3(std::vector<float> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    };

    static void insertVec2(std::vector<float> &data, glm::vec2 v) {
        data.push_back(v.x);
        data.push_back(v.y);
    }
};
