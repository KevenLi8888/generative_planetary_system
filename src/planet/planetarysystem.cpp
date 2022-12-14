#include "planet/planetarysystem.h"
#include "glm/gtx/transform.hpp"

#include <iostream>

struct SolarSystemPlanet {
    std::string texture_fname;
    float diameter;             // km
    float rotational_velocity;  // km/h
    float orbital_radius;
    float orbital_velocity;
    float orbital_inclination;
    int num_moons;
};

SolarSystemPlanet Sun {
    "resources/images/sun.jpeg",
    1392684,
    0,
    0,
    0,
    0,
    0
};

//    SolarSystemPlanet {
//            // texture_fname;
//            // diameter;
//            // rotational_velocity;
//            // orbital_radius;
//            // orbital_velocity;
//            // orbital_inclination;
//            // num_moons;
//    },

std::vector<SolarSystemPlanet> Planets {
    // Mercury
    SolarSystemPlanet {
        "resources/images/mercury.jpeg",
        4879,
        10.83,
        57.9,
        47.4,
        7.0,
        0
    },
    // Venus
    SolarSystemPlanet {
        "resources/images/venus.jpeg",
        12104,
        6.52,
        108.2,
        35,
        3.4,
        0
    },
    // Earth
    SolarSystemPlanet {
        "resources/images/earth.jpeg",
        12756,
        1574,
        149.6,
        29.8,
        0,
        1,
    },
    // Mars
    SolarSystemPlanet {
        "resources/images/mars.jpeg",
        6792,
        866,
        228,
        24.1,
        1.8,
        2,
    },
    // Jupiter
    SolarSystemPlanet {
        "resources/images/jupiter.jpeg",
        142984,
        45583,
        778.5,
        13.1,
        1.3,
        79,
    },
    // Saturn
    SolarSystemPlanet {
        "resources/images/saturn.jpeg",
        120536,
        36840,
        1432,
        9.7,
        2.5,
        82,
    },
    // Uranus
    SolarSystemPlanet {
        "resources/images/uranus.jpeg",
        51118,
        14798,
        2867,
        6.8,
        0.8,
        27,
    },
    // Neptune
    SolarSystemPlanet {
        "resources/images/neptune.jpeg",
        49528,
        9719,
        4515,
        5.4,
        1.8,
        14,
    },
};

void deletePlanet(Planet *curr) {
    for (auto &p : curr->getChildren()) {
        deletePlanet(p);
    }
    delete curr;
}

PlanetarySystem::~PlanetarySystem() {
    if (m_root != nullptr) deletePlanet(m_root);
}

float scaleDiameter(float diameter) {
    return (log10(diameter) - 3) * 0.5;
}

float scaleOrbitalRadius(float radius) {
    return (log10(radius) - 1.2) * 7;
}

float scaleVelocity(float v) {
    return log(v) / 3;
}

std::vector<RenderShapeData*> PlanetarySystem::generateSolarSystem() {
    std::vector<RenderShapeData*> data;

    SceneMaterial mat;
    mat.cDiffuse = glm::vec4(1, 1, 1, 0);
    mat.blend = 1;
    mat.textureMap.isUsed = true;
    mat.textureMap.repeatU = 1;
    mat.textureMap.repeatV = 1;

    // Add sun
    mat.textureMap.filename = Sun.texture_fname;
    RenderShapeData *sun_shape = new RenderShapeData {{PrimitiveType::PRIMITIVE_SPHERE, mat}, glm::mat4(1)};
    Planet *sun = new Planet(scaleDiameter(Sun.diameter), 0, 0, 0, 0, glm::vec3(0, 1, 0), sun_shape);
    std::vector<Planet*> sun_children;
    data.push_back(sun_shape);

    // Add planets
    mat.blend = 0.5;
    for (auto &planet: Planets) {
        mat.textureMap.filename = planet.texture_fname;
        RenderShapeData *p_shape = new RenderShapeData {{PrimitiveType::PRIMITIVE_SPHERE, mat}, glm::mat4(1)};
        Planet *p = new Planet(scaleDiameter(planet.diameter),
                               scaleVelocity(planet.orbital_velocity),
                               scaleVelocity(planet.rotational_velocity / planet.diameter),
                               0,
                               scaleOrbitalRadius(planet.orbital_radius),
                               computeAxis(planet.orbital_inclination),
                               p_shape);
        sun_children.push_back(p);
        data.push_back(p_shape);
        p->setParent(sun);
    }

    sun->setChildren(sun_children);

    m_root = sun;

    return data;
}

// https://nssdc.gsfc.nasa.gov/planetary/factsheet/
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

    data.push_back(sun_shape);
    data.push_back(mars_shape);

    Planet *sun = new Planet(2, 0, 0, 0, 0, glm::vec3(0, 1, 0), sun_shape);
    Planet *mars = new Planet(0.75, 0.5, 1, 0.1, 5, computeAxis(1.8f), mars_shape);


    sun->setChildren({ mars });
    mars->setParent(sun);

    m_root = sun;

    return data;
}

void PlanetarySystem::update(float deltaTime) {
    m_root->updateCTM(deltaTime);
}

void getOrbitCtmsHelper(std::vector<glm::mat4> &ctms, Planet *p) {
    auto parent = p->getParent();

    if (parent != nullptr) {
        ctms.push_back(parent->getTranslateMat() * glm::scale(glm::vec3(2 * p->getOrbitRadius())) * p->getOrientMat() * glm::mat4(1));
    }

    for (auto &child: p->getChildren()) {
        getOrbitCtmsHelper(ctms, child);
    }
}

std::vector<glm::mat4> PlanetarySystem::getOrbitCtms() {
    std::vector<glm::mat4> orbit_ctms;

    getOrbitCtmsHelper(orbit_ctms, m_root);

    return orbit_ctms;
}

// Get rotational axis based on inclination degree
glm::vec3 PlanetarySystem::computeAxis(float inclination) {
    glm::vec3 DEFAULT_AXIS = glm::vec3(0, 1, 0);
    if (inclination == 0) return DEFAULT_AXIS;

    auto rotation_mat = glm::rotate(glm::mat4(1), glm::radians(inclination), glm::vec3(1, 0, 0));
    return rotation_mat * glm::vec4(DEFAULT_AXIS, 0);
}
