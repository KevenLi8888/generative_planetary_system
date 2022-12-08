#pragma once

#include "shape/shape.h"

class Cylinder : public Shape
{
public:
    static std::vector<float> generateShape(int param1, int param2);

private:
    inline static std::vector<float> m_vertexData;

    static void makeCapTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight,
                     glm::vec3 normal);
    static void makeSideTile(glm::vec2 left,
                      glm::vec2 right,
                      float topY,
                      float bottomY);
    static void makeCapsSlice(glm::vec2 left, glm::vec2 right, int param1);
    static void makeSideSlice(glm::vec2 left, glm::vec2 right, int param1);
    static void makeCylinder(int param1, int param2);

    inline static const int MIN_PARAM1 = 1;
    inline static const int MIN_PARAM2 = 3;
};
