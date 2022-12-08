#pragma once

#include "shape/shape.h"

class Sphere : public Shape
{
public:
    // All derived classes implement this function to generate vertex data
    static std::vector<float> generateShape(int param1, int param2);

private:
    inline static std::vector<float> m_vertexData;

    static void makeTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight);
    static void makeWedge(float currTheta, float nextTheta, int param1);
    static void makeSphere(int param1, int param2);

    inline static const int MIN_PARAM1 = 2;
    inline static const int MIN_PARAM2 = 3;
};
