#include "shape/cone.h"
#include "utils/texturemap.h"

void Cone::makeBaseTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight,
                       glm::vec3 normal) {
    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CONE));

    // Lower triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CONE));
}

void Cone::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        glm::vec3 leftNormal,
                        glm::vec3 rightNormal) {
    // Special case at the tip
    auto topLeftNormal = leftNormal;
    auto topRightNormal = rightNormal;

    // Subtract epsilon from boundary because the tip advanced epsilon unit
    if (topLeft.y >= Constants::BOUNDARY - Constants::EPSILON) {
        auto tipNormal = glm::normalize(leftNormal + rightNormal);
        topLeftNormal = tipNormal;
        topRightNormal = tipNormal;
    }

    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, topLeftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, leftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, topRightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CONE));

    // Lower triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, topRightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, leftNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_CONE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, rightNormal);
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_CONE));
}

void Cone::makeBaseSlice(glm::vec2 left, glm::vec2 right, int param1) {
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

        makeBaseTile(glm::vec3(currRight.x, -0.5, currRight.y),
                     glm::vec3(currLeft.x, -0.5, currLeft.y),
                     glm::vec3(nextRight.x, -0.5, nextRight.y),
                     glm::vec3(nextLeft.x, -0.5, nextLeft.y),
                     glm::vec3(0, -1, 0));

        currLeft = nextLeft;
        currRight = nextRight;
    }
}
void Cone::makeSideSlice(glm::vec2 left, glm::vec2 right, int param1) {
    // Direction vectors for 2D iteration
    glm::vec3 toLeft(left.x, -1, left.y);
    glm::vec3 toRight(right.x, -1, right.y);

    // Start from the tip and iterate towards the base
    glm::vec3 currLeft(0, 0.5, 0), currRight(0, 0.5, 0);

    // Avoid tip boundary case
    currLeft += toLeft * Constants::EPSILON;
    currRight += toRight * Constants::EPSILON;

    // Compute a unit step from the tip towards the base
    auto leftStep = toLeft * (1 - 2 * Constants::EPSILON) / (float)param1;
    auto rightStep = toRight * (1 - 2 * Constants::EPSILON) / (float)param1;

    // Calculate the normals on the left and right edge of the triangular side section
    glm::vec3 leftNormal(left.x, sqrt(pow(left.x, 2) + pow(left.y, 2)) * 0.5, left.y);
    glm::vec3 rightNormal(right.x, sqrt(pow(right.x, 2) + pow(right.y, 2)) * 0.5, right.y);
    leftNormal = glm::normalize(leftNormal);
    rightNormal = glm::normalize(rightNormal);

    for (int i = 0; i < param1; ++i) {
        auto nextLeft = currLeft + leftStep;
        auto nextRight = currRight + rightStep;

        makeSideTile(currLeft, currRight, nextLeft, nextRight, leftNormal, rightNormal);

        currLeft = nextLeft;
        currRight = nextRight;
    }
}

void Cone::makeCone(int param1, int param2) {
    // First "slice" the cone vertically by delta theta
    float thetaStep = glm::radians((360.f - 2 * Constants::EPSILON) / param2);
    float currentTheta = glm::radians(Constants::EPSILON);

    for (int i = 0; i < param2; ++i) {
        // Compute the (x, z) coordinate of the left and right vertices
        auto nextTheta = currentTheta + thetaStep;
        auto left = glm::vec2(glm::cos(nextTheta), glm::sin(nextTheta)) * 0.5f;
        auto right = glm::vec2(glm::cos(currentTheta), glm::sin(currentTheta)) * 0.5f;

        makeBaseSlice(left, right, param1);
        makeSideSlice(left, right, param1);
        currentTheta = nextTheta;
    }
}

std::vector<float> Cone::generateShape(int param1, int param2) {
    m_vertexData.clear();
    makeCone(std::max(param1, MIN_PARAM1), std::max(param2, MIN_PARAM2));
    return m_vertexData;
}
