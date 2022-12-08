#pragma once

#include "shape/shape.h"

class Cube : public Shape
{
public:
    static std::vector<float> generateShape(int param1, int param2);

private:
    inline static std::vector<float> m_vertexData;

    static void makeTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight);
    static void makeFace(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight,
                         int param1);
    static void makeCube(int param1);

    inline static const int MIN_PARAM1 = 1;
};
