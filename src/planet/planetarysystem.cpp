#include "planet/planetarysystem.h"

#include <iostream>

void deletePlanet(Planet *curr) {
    for (auto &p : curr->m_children) {
        deletePlanet(p);
    }
    delete curr;
}

PlanetarySystem::~PlanetarySystem() {
    if (m_root != nullptr) deletePlanet(m_root);
}

std::vector<RenderShapeData*> PlanetarySystem::generateSystem() {
    std::vector<RenderShapeData*> data;

    SceneMaterial mat;
    mat.cDiffuse = glm::vec4(1, 1, 1, 0);
    mat.blend = 0.75;
    mat.textureMap.isUsed = true;
    mat.textureMap.filename = "resources/images/marsTexture.png";
    mat.textureMap.repeatU = 1;
    mat.textureMap.repeatV = 1;

    RenderShapeData *sun_shape = new RenderShapeData {{PrimitiveType::PRIMITIVE_SPHERE, mat}, glm::mat4(1)};
    RenderShapeData *mars_shape = new RenderShapeData {{PrimitiveType::PRIMITIVE_SPHERE, mat}, glm::mat4(1)};
    RenderShapeData *moon_shape = new RenderShapeData {{PrimitiveType::PRIMITIVE_SPHERE, mat}, glm::mat4(1)};

    data.push_back(sun_shape);
    data.push_back(mars_shape);
    data.push_back(moon_shape);

    Planet *sun = new Planet(2, 0, 0, 0, 0, glm::vec3(0, 1, 0), sun_shape);
    Planet *mars = new Planet(0.75, 0.5, 1, 0.1, 5, glm::vec3(0, 1, 0), mars_shape);
    Planet *moon = new Planet(0.2, 2, 1, 0.5, 1, glm::vec3(0, 1, 0), moon_shape);

    sun->setChildren({ mars });
    mars->setChildren({ moon });
    mars->setParent(sun);
    moon->setParent(mars);

    m_root = sun;

    return data;
}

void PlanetarySystem::update(float deltaTime) {
    m_root->updateCTM(deltaTime);
}
