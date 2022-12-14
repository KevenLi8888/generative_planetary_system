#pragma once

#include "planet/planet.h"

class PlanetarySystem {
public:
    ~PlanetarySystem();
    std::vector<RenderShapeData*> generateSystem();
    std::vector<RenderShapeData*> generateSolarSystem();
    void update(float deltaTime);
    std::vector<glm::mat4> getOrbitCtms();

private:
    Planet *m_root;

    glm::vec3 computeAxis(float inclination);
};
