#pragma once

#include "utils/scenedata.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <QOpenGLWidget>

class Camera {
public:
    Camera() = default;
    Camera(int width, int height, SceneCameraData data);

    // Update the camera upon window resizing
    void resize(int width, int height);

    // Update the camera according to keyboard input
    void moveCamera(std::unordered_map<Qt::Key, bool> &key_map, float dist);

    // Update the camera according to mouse input
    void rotateCamera(float dx, float dy);

    // Returns the view matrix for the current camera settings
    glm::mat4 getViewMatrix() const { return m_view; };

    // Returns the projection matrix for the current camera settings
    glm::mat4 getProjectionMatrix() const { return m_proj; };

    // Returns the current camera position
    glm::vec4 getPosition() const { return glm::vec4(m_pos, 1); };

private:
    glm::vec3 m_look;
    glm::vec3 m_pos;
    glm::vec3 m_up;

    glm::mat4 m_view;
    glm::mat4 m_proj;
    float m_aspectRatio;
    float m_heightAngle;
    float m_widthAngle;

    void updateView();
    void updateProjection();
};
