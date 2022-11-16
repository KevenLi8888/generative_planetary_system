#pragma once

#include <glm/glm.hpp>
#include "scenedata.h"

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    Camera(int width, int height, const SceneCameraData &data, float near, float far);

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInversedViewMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;
    float getWidthAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::vec4 getCameraPosition() const;

    glm::mat4 getProjectionMatrix() const;

    void updateCamaraSize(int w, int h);

    void updateNearPlane(double near);

    void updateFarPlane(double far);

private:
    SceneCameraData camera_data;
    glm::mat4 inversed_view_matrix;
    int camera_width, camera_height;
    double near_plane, far_plane;
};
