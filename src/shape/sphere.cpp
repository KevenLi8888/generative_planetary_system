#include "shape/sphere.h"
#include "utils/texturemap.h"

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Upper triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_SPHERE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_SPHERE));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec2(m_vertexData, TextureMap::getUVAt(topRight, PrimitiveType::PRIMITIVE_SPHERE));

    // Lower triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec2(m_vertexData, TextureMap::getUVAt(topLeft, PrimitiveType::PRIMITIVE_SPHERE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomLeft, PrimitiveType::PRIMITIVE_SPHERE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec2(m_vertexData, TextureMap::getUVAt(bottomRight, PrimitiveType::PRIMITIVE_SPHERE));
}

void Sphere::makeWedge(float currentTheta, float nextTheta, int param1) {
    float phiStep = glm::radians((180.f - 2 * Constants::EPSILON) / param1);
    float currentPhi = glm::radians(Constants::EPSILON);

    // Divide the wedge vertically into <param1> tiles
    for (int i = 0; i < param1; ++i) {
        auto nextPhi = currentPhi + phiStep;
        glm::vec3 topRight(0.5 * glm::sin(currentPhi) * glm::cos(currentTheta),
                           0.5 * glm::cos(currentPhi),
                           0.5 * glm::sin(currentPhi) * glm::sin(currentTheta));
        glm::vec3 topLeft(0.5 * glm::sin(currentPhi) * glm::cos(nextTheta),
                          0.5 * glm::cos(currentPhi),
                          0.5 * glm::sin(currentPhi) * glm::sin(nextTheta));
        glm::vec3 bottomRight(0.5 * glm::sin(nextPhi) * glm::cos(currentTheta),
                              0.5 * glm::cos(nextPhi),
                              0.5 * glm::sin(nextPhi) * glm::sin(currentTheta));
        glm::vec3 bottomLeft(0.5 * glm::sin(nextPhi) * glm::cos(nextTheta),
                             0.5 * glm::cos(nextPhi),
                             0.5 * glm::sin(nextPhi) * glm::sin(nextTheta));
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
        currentPhi = nextPhi;
    }
}

void Sphere::makeSphere(int param1, int param2) {
    float thetaStep = glm::radians((360.f - 2 * Constants::EPSILON) / param2);
    float currentTheta = glm::radians(Constants::EPSILON);

    // Divide the sphere horizontally into <param2> wedges
    for (int i = 0; i < param2; ++i) {
        auto nextTheta = currentTheta + thetaStep;
        makeWedge(currentTheta, nextTheta, param1);
        currentTheta = nextTheta;
    }
}

std::vector<float> Sphere::generateShape(int param1, int param2) {
    m_vertexData.clear();
    makeSphere(std::max(param1, MIN_PARAM1), std::max(param2, MIN_PARAM2));
    return m_vertexData;
}
