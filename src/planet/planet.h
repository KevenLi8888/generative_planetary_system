#pragma once

#include "utils/scenedata.h"

class Planet {
public:
    Planet(float diameter,
           float orbit_v,
           float revolve_v,
           float initial_theta,
           float orbit_radius,
           glm::vec3 orbit_axis,
           RenderShapeData *shape);

    void setParent(Planet *p);
    void setChildren(std::vector<Planet*> cs) { m_children = cs; };
    void updateCTM(float deltaTheta);
    void updateOrbits();

    Planet *getParent() const { return m_parent; };
    std::vector<Planet*> getChildren() const { return m_children; };

    glm::mat4 getTranslateMat() const { return m_translate_mat; };
    glm::mat4 getOrientMat() const { return m_orient_mat; };
    float getOrbitRadius() const { return m_orbit_radius; };

private:
    void updateCTMHelper(float deltaTheta, glm::vec3 parent_pos);

    Planet *m_parent;
    std::vector<Planet*> m_children;

    RenderShapeData *m_shape;

    glm::vec4 m_orbit_start;
    glm::vec3 m_orbit_axis;
    glm::mat4 m_scale_mat;
    glm::mat4 m_orient_mat;
    glm::mat4 m_revolve_mat;
    glm::mat4 m_translate_mat;

    float m_orbit_radius;
    float m_orbit_v;
    float m_revolve_v;
    float m_revolve_theta;
    float m_orbit_theta;
};
