#include "shape/ring.h"

void Ring::makeRing(int param1) {
    float theta_step = 2 * glm::radians(360.f) / param1;
    for (int i = 0; i <= param1; ++i) {
        insertVec3(m_vertexData, glm::vec3(0.5 * cos(theta_step * i), 0, 0.5 * sin(theta_step * i)));
    }
}

std::vector<float> Ring::generateShape(int param1, int param2) {
    m_vertexData.clear();
    makeRing(std::max(param1, MIN_PARAM1) * 100);
    return m_vertexData;
}
