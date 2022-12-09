#pragma once

#include "planet/planet.h"

class PlanetarySystem {
public:
    ~PlanetarySystem();
    std::vector<RenderShapeData*> generateSystem();
    void update(float deltaTime);
private:
    Planet *m_root;
};
