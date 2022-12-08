#include "shape/cube.h"
#include "utils/texturemap.h"

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft)));
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CUBE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight)));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CUBE));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight)));
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CUBE));

    // Lower triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft)));
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CUBE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CUBE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight)));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CUBE));
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    int param1) {
    // Direction vectors for 2D iteration
    auto toRight = (topRight - topLeft);
    auto toBottom = (bottomLeft - topLeft);

    // Avoid edge boundary case (ok with even smaller epsilon)
    float epsilon = Constants::EPSILON / 100.f;
    topLeft += epsilon * (toRight + toBottom);

    // Assuming that all four vertices are on the same plane
    auto rightStep = toRight * (1 - 2 * epsilon) / (float)param1;
    auto downStep = toBottom * (1 - 2 * epsilon) / (float)param1;

    // Iterate through the grid like in 2D
    for (int r = 0; r < param1; ++r) {
        auto currTopLeft = topLeft + (float)r * downStep;
        for (int c = 0; c < param1; ++c) {
            auto currTopRight = currTopLeft + rightStep;
            auto currBottomLeft = currTopLeft + downStep;
            auto currBottomRight = currTopRight + downStep;
            makeTile(currTopLeft, currTopRight, currBottomLeft, currBottomRight);
            currTopLeft += rightStep;
        }
    }
}

void Cube::makeCube(int param1) {
    // Fix Z
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             param1);
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             param1);

    // Fix Y
    makeFace(glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             param1);

    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             param1);

    // Fix X
    makeFace(glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             param1);

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             param1);
}

std::vector<float> Cube::generateShape(int param1, int param2) {
    m_vertexData.clear();
    makeCube(std::max(param1, MIN_PARAM1));
    return m_vertexData;
}
