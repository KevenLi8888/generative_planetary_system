#pragma once

#include "shape/shape.h"

class Ring : public Shape
{
public:
    // All derived classes implement this function to generate vertex data
    static std::vector<float> generateShape(int param1, int param2);

private:
    inline static std::vector<float> m_vertexData;

    static void makeRing(int param1);

    inline static const int MIN_PARAM1 = 5;
};
