#pragma once

#include "shape/shape.h"

class Cone : public Shape
{
public:
    static std::vector<float> generateShape(int param1, int param2);

private:
    inline static std::vector<float> m_vertexData;

    static void makeBaseTile(glm::vec3 topLeft,
                             glm::vec3 topRight,
                             glm::vec3 bottomLeft,
                             glm::vec3 bottomRight,
                             glm::vec3 normal);
    static void makeSideTile(glm::vec3 topLeft,
                             glm::vec3 topRight,
                             glm::vec3 bottomLeft,
                             glm::vec3 bottomRight,
                             glm::vec3 leftNormal,
                             glm::vec3 rightNormal);
    static void makeBaseSlice(glm::vec2 left, glm::vec2 right, int param1);
    static void makeSideSlice(glm::vec2 left, glm::vec2 right, int param1);
    static void makeCone(int param1, int param2);

    inline static const int MIN_PARAM1 = 1;
    inline static const int MIN_PARAM2 = 3;
};
