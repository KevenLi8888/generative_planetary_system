#include "camera/camera.h"
#include "settings.h"
#include <iostream>

Camera::Camera(int width, int height, SceneCameraData data) {
    m_look = glm::normalize(data.look);
    m_pos = data.pos;
    m_up = glm::normalize(data.up);
    m_heightAngle = data.heightAngle;

    resize(width, height);
    updateView();
}

// Update the camera upon window resizing
void Camera::resize(int width, int height) {
    // Calculate the width angle using trigonometry
    m_aspectRatio = (float) width / height;
    m_widthAngle = glm::atan(glm::tan(m_heightAngle / 2) * m_aspectRatio) * 2;

    updateProjection();
}

// Update the camera according to keyboard input
void Camera::moveCamera(std::unordered_map<Qt::Key, bool> &key_map, float dist) {    
    if (settings.GPS && settings.orbitCamera) {
        auto prev_dist = m_distance;

        if (key_map[Qt::Key_W]) m_distance -= dist;
        if (key_map[Qt::Key_S]) m_distance += dist;

        m_distance = std::max(m_distance, 0.f);

        if (m_distance != prev_dist) {
            updatePlanetLPU();
            updateView();
        }
    } else {
        auto prev_pos = m_pos;
        auto left = glm::cross(m_up, m_look);

        if (key_map[Qt::Key_W]) m_pos += dist * m_look;
        if (key_map[Qt::Key_S]) m_pos -= dist * m_look;
        if (key_map[Qt::Key_A]) m_pos += dist * left;
        if (key_map[Qt::Key_D]) m_pos -= dist * left;
        if (key_map[Qt::Key_Space]) m_pos += dist * glm::vec3(0, 1, 0);
        if (key_map[Qt::Key_Control]) m_pos += dist * glm::vec3(0, -1, 0);

        if (m_pos != prev_pos) updateView();
    }
}

// v: vector to rotate, u: normalized axis, theta: angle in radians
inline glm::vec3 rotate(glm::vec3 v, glm::vec3 u, float theta) {
    float c = cos(theta), s = sin(theta);

    // Using the Rogdigues' formula for rotation
    return glm::mat3(
        c + u.x * u.x * (1 - c), u.x * u.y * (1 - c) + u.z * s, u.x * u.z * (1 - c) - u.y * s,
        u.x * u.y * (1 - c) - u.z * s, c + u.y * u.y * (1 - c), u.y * u.z * (1 - c) + u.x * s,
        u.x * u.z * (1 - c) + u.y * s, u.y * u.z * (1 - c) - u.x * s, c + u.z * u.z * (1 - c)
    ) * v;
}

// Update the camera according to mouse input
void Camera::rotateCamera(float dx, float dy) {
    if (settings.GPS && settings.orbitCamera) {
        m_theta = std::fmod(m_theta - dx, 2 * glm::pi<float>());
        m_phi = std::min(std::max(m_phi + dy, 0.001f), glm::pi<float>() - 0.001f);

        updatePlanetLPU();
    } else {
        m_look = rotate(m_look, glm::vec3(0, 1, 0), -dx);
        m_look = rotate(m_look, glm::normalize(glm::cross(m_look, m_up)), -dy);
    }
    updateView();
}

void Camera::updateView() {
    // Compute the u, v, and w vectors based on look, up, and pos
    glm::vec3 w = glm::normalize(-m_look);
    glm::vec3 v = glm::normalize(m_up - glm::dot(m_up, w) * w);
    glm::vec3 u = glm::cross(v, w);

    // Compute the rotation matrix
    glm::mat4 M_rot(
        u.x, v.x, w.x, 0,
        u.y, v.y, w.y, 0,
        u.z, v.z, w.z, 0,
        0, 0, 0, 1
    );

    // Compute the translation matrix
    glm::mat4 M_t(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -m_pos.x, -m_pos.y, -m_pos.z, 1
    );

    m_view = M_rot * M_t;
}

void Camera::updateProjection() {
    // Compute the scale xyz matrix
    glm::mat4 Sxyz(1.0);

    float inv_far = 1.0 / settings.farPlane;
    Sxyz[0][0] = inv_far / tan(m_widthAngle / 2);
    Sxyz[1][1] = inv_far / tan(m_heightAngle / 2);
    Sxyz[2][2] = inv_far;

    // Compute the unhinging transformation maatrix
    glm::mat4 Mpp(1.0);

    float c = -settings.nearPlane / settings.farPlane;
    Mpp[2][2] = 1 / (1 + c);
    Mpp[3][2] = -c / (1 + c);
    Mpp[2][3] = -1;
    Mpp[3][3] = 0;

    // Compute the z remapping matrix
    glm::mat4 Rz(1.0);
    Rz[2][2] = -2;
    Rz[3][2] = -1;

    // Putting them together
    m_proj = Rz * Mpp * Sxyz;
}

void Camera::resetCameraOrbit() {
    m_distance = DEFAULT_DISTANCE;
    m_theta = DEFAULT_THETA;
    m_phi = DEFAULT_PHI;

    m_look_planet = DEFAULT_LOOK_PLANET;
    m_pos_planet = DEFAULT_POS_PLANET;
    m_up_planet = DEFAULT_UP_PLANET;

    updatePlanetLPU();
}

void Camera::updatePlanetLPU() {
    // Compute look vector in the planet object space
    m_look_planet = -DEFAULT_LOOK_PLANET;
    m_look_planet = rotate(m_look_planet, DEFAULT_UP_PLANET, m_theta);
    m_look_planet = rotate(m_look_planet, glm::normalize(glm::cross(m_look_planet, DEFAULT_UP_PLANET)), m_phi - DEFAULT_PHI);
    m_look_planet = -glm::normalize(m_look_planet);

    // Compute pos vector in the planet object space
    m_pos_planet = -m_look_planet * m_distance;
}

void Camera::updateCameraView(RenderShapeData *planet) {
    m_look = planet->ctm * glm::vec4(m_look_planet, 0);
    m_pos = planet->ctm * glm::vec4(m_pos_planet, 1);
    m_up = planet->ctm * glm::vec4(m_up_planet, 0);
    updateView();
}
