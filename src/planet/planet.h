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

    Planet *m_parent;
    std::vector<Planet*> m_children;

private:
    void updateCTMHelper(float deltaTheta, glm::vec3 parent_pos);
    glm::mat4 getScale();
    glm::mat4 getPosition();
    glm::mat4 getRotation();

    RenderShapeData *m_shape;
    glm::vec4 m_orbit_start;
    glm::vec3 m_orbit_axis;
    glm::mat4 m_scale_mat;
    glm::mat4 m_orient_mat;
    float m_orbit_radius;
    float m_orbit_v;
    float m_revolve_v;
    float m_revolve_theta;
    float m_orbit_theta;
};
