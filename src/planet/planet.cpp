#include "planet/planet.h"

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>

glm::mat4 computeOrientMat(glm::vec3 axis) {
    if (axis == glm::vec3(0, 1, 0)) return glm::mat4(1);
    auto new_y = glm::normalize(axis);
    auto new_z = glm::normalize(glm::cross(new_y, glm::vec3(0, 1, 0)));
    auto new_x = glm::normalize(glm::cross(new_y, new_z));
    return glm::mat4(
                glm::vec4(new_x, 0),
                glm::vec4(new_y, 0),
                glm::vec4(new_z, 0),
                glm::vec4(0, 0, 0, 1)
    );
}

Planet::Planet(float diameter,
               float orbit_v,
               float revolve_v,
               float initial_theta,
               float orbit_radius,
               glm::vec3 orbit_axis,
               RenderShapeData *shape)
{
    m_orbit_theta = initial_theta;
    m_revolve_theta = initial_theta * revolve_v;
    m_revolve_v = revolve_v;
    m_orbit_v = orbit_v;
    m_orbit_radius = orbit_radius;
    m_orbit_axis = orbit_axis;
    m_shape = shape;

    m_scale_mat = glm::scale(glm::vec3(diameter));
    m_orient_mat = computeOrientMat(orbit_axis);
}

void Planet::setParent(Planet *p) {
    m_orbit_start = glm::vec4(glm::cross(m_orbit_axis, glm::vec3(1, 0, 0)), 1);
    m_parent = p;
}

void Planet::updateCTMHelper(float deltaTheta, glm::vec3 parent_pos) {
    m_orbit_theta += std::fmod(deltaTheta * m_orbit_v, 2 * glm::pi<float>());

    glm::vec3 curr_pos = parent_pos;
    m_revolve_mat = glm::mat4(1);

    if (m_parent != nullptr) {
        m_revolve_theta += std::fmod(deltaTheta * m_revolve_v, 2 * glm::pi<float>());
        m_revolve_mat = glm::rotate(glm::mat4(1), m_revolve_theta, m_orbit_axis);

        auto orbit_mat = glm::rotate(glm::mat4(1), m_orbit_theta, m_orbit_axis);
        curr_pos += glm::vec3(m_orbit_start * orbit_mat * m_orbit_radius);
    }

    m_translate_mat = glm::translate(curr_pos);

    m_shape->ctm = m_translate_mat * m_scale_mat * m_revolve_mat * m_orient_mat;

    for (auto &p : m_children) {
        p->updateCTMHelper(deltaTheta, curr_pos);
    }
}

void Planet::updateCTM(float deltaTheta) {
    updateCTMHelper(deltaTheta, glm::vec3(0));
}
