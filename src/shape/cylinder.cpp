#include "cylinder.h"
#include "utils/texturemap.h"

void Cylinder::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight,
                           glm::vec3 normal) {
    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CYLINDER));

    // Lower triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CYLINDER));
}

void Cylinder::makeSideTile(glm::vec2 left,
                            glm::vec2 right,
                            float topY,
                            float bottomY) {
    // Calculate vertices and normals
    glm::vec3 topLeft(left.x, topY, left.y);
    glm::vec3 topRight(right.x, topY, right.y);
    glm::vec3 bottomLeft(left.x, bottomY, left.y);
    glm::vec3 bottomRight(right.x, bottomY, right.y);

    auto leftNormal = glm::vec3(left.x, 0, left.y) * 2.f;
    auto rightNormal = glm::vec3(right.x, 0, right.y) * 2.f;

    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, leftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, leftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, rightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CYLINDER));

    // Lower triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, rightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, leftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CYLINDER));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, rightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CYLINDER));
}

void Cylinder::makeCapsSlice(glm::vec2 left, glm::vec2 right, int param1) {
    // Unit steps for top and bottom caps
    float n = param1;
    auto leftStep = left / n;
    auto rightStep = right / n;

    // Starting positions for top and bottom caps
    glm::vec2 currLeft(0), currRight(0);

    // Build the slice for the top and the bottom cap starting from the rim inward
    for (int i = 0; i < param1; ++i) {
        auto nextLeft = currLeft + leftStep;
        auto nextRight = currRight + rightStep;

        makeCapTile(glm::vec3(currLeft.x, 0.5, currLeft.y),
                    glm::vec3(currRight.x, 0.5, currRight.y),
                    glm::vec3(nextLeft.x, 0.5, nextLeft.y),
                    glm::vec3(nextRight.x, 0.5, nextRight.y),
                    glm::vec3(0, 1, 0));
        makeCapTile(glm::vec3(currRight.x, -0.5, currRight.y),
                    glm::vec3(currLeft.x, -0.5, currLeft.y),
                    glm::vec3(nextRight.x, -0.5, nextRight.y),
                    glm::vec3(nextLeft.x, -0.5, nextLeft.y),
                    glm::vec3(0, -1, 0));

        currLeft = nextLeft;
        currRight = nextRight;
    }
}
void Cylinder::makeSideSlice(glm::vec2 left, glm::vec2 right, int param1) {
    // Cut the side slice horizontally according to parameter 1
    float topY = 0.5 - Constants::EPSILON;
    float yStep = (1 - 2 * Constants::EPSILON) / (float)param1;

    for (int i = 0; i < param1; ++i) {
        auto bottomY = topY - yStep;

        makeSideTile(left, right, topY, bottomY);

        topY = bottomY;
    }
}

void Cylinder::makeCylinder(int param1, int param2) {
    // First "slice" the cylinder vertically by delta theta
    float thetaStep = glm::radians((360.f - 2 * Constants::EPSILON) / param2);
    float currentTheta = glm::radians(Constants::EPSILON);

    for (int i = 0; i < param2; ++i) {
        // Compute the (x, z) coordinate of the left and right vertices
        auto nextTheta = currentTheta + thetaStep;
        auto left = glm::vec2(glm::cos(nextTheta), glm::sin(nextTheta)) * 0.5f;
        auto right = glm::vec2(glm::cos(currentTheta), glm::sin(currentTheta)) * 0.5f;

        makeCapsSlice(left, right, param1);
        makeSideSlice(left, right, param1);
        currentTheta = nextTheta;
    }
}

std::vector<float> Cylinder::generateShape(int param1, int param2) {
    m_vertexData.clear();
    makeCylinder(std::max(param1, MIN_PARAM1), std::max(param2, MIN_PARAM2));
    return m_vertexData;
}
