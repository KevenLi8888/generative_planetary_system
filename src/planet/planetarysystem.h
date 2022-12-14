#pragma once

#include "planet/planet.h"

class PlanetarySystem {
public:
    ~PlanetarySystem();
    std::vector<RenderShapeData*> generateSolarSystem();
    std::vector<RenderShapeData*> generateProceduralSystem();
    void update(float deltaTime);
    std::vector<glm::mat4> getOrbitCtms();
    int getNumMoon() const { return m_num_moon; };

private:
    Planet *m_root;
    int m_num_moon;

    glm::vec3 computeAxis(float inclination);
};
