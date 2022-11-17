#include <stdexcept>
#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(int width, int height, const SceneCameraData &data, float near, float far):
        camera_width(width), camera_height(height), camera_data(data), near_plane(double(near)), far_plane(double(far)) {
    inversed_view_matrix = glm::inverse(getViewMatrix());
}

glm::mat4 Camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    // Lab 4: https://cs1230.graphics/labs/lab4#camera-view-matrix

    glm::vec3 look = camera_data.look, up = camera_data.up, pos = camera_data.pos;
    glm::vec3 u, v, w;
    w = -glm::normalize(look);
    v = glm::normalize(up - dot(up, w) * w);
    u = glm::cross(v, w);

    glm::mat4 rotate;
    rotate[0] = glm::vec4(u[0], v[0], w[0], 0.f);
    rotate[1] = glm::vec4(u[1], v[1], w[1], 0.f);
    rotate[2] = glm::vec4(u[2], v[2], w[2], 0.f);
    rotate[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);

    glm::mat4 translate(1.f);
    translate[3] = glm::vec4(-pos[0], -pos[1], -pos[2], 1);

    return rotate * translate;
}

glm::mat4 Camera::getInversedViewMatrix() const {
    return inversed_view_matrix;
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    // https://edstem.org/us/courses/24281/discussion/1916850
    return float(camera_width) / float(camera_height);
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return camera_data.heightAngle;
}

float Camera::getWidthAngle() const {
    float aspect_ratio = getAspectRatio();
    float view_height = 2 * tan(camera_data.heightAngle/2);
    float view_width = aspect_ratio * view_height;
    return 2 * atan(view_width/2);
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    return camera_data.focalLength;
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    return camera_data.aperture;
}

glm::vec4 Camera::getCameraPosition() const {
    return camera_data.pos;
}

glm::mat4 Camera::getProjectionMatrix() const {
    glm::mat4 Sxyz, Mpp, Mz;

    Sxyz[0] = glm::vec4(1/(far_plane * tan(getWidthAngle()/2)), 0, 0, 0);
    Sxyz[1] = glm::vec4(0, 1/(far_plane * tan(getHeightAngle()/2)), 0, 0);
    Sxyz[2] = glm::vec4(0, 0, 1/far_plane, 0);
    Sxyz[3] = glm::vec4(0, 0, 0, 1);

    auto c = - near_plane / far_plane;
    Mpp[0] = glm::vec4(1, 0, 0, 0);
    Mpp[1] = glm::vec4(0, 1, 0, 0);
    Mpp[2] = glm::vec4(0, 0, 1/(1+c), -1);
    Mpp[3] = glm::vec4(0, 0, -c/(1+c), 0);

    Mz[0] = glm::vec4(1, 0, 0, 0);
    Mz[1] = glm::vec4(0, 1, 0, 0);
    Mz[2] = glm::vec4(0, 0, -2, 0);
    Mz[3] = glm::vec4(0, 0, -1, 1);

    return Mz * Mpp * Sxyz;
    // return glm::perspective(double(camera_data.heightAngle),1.0 * camera_width / camera_height,near_plane,far_plane);
}

glm::mat4 Camera::getSxyzMatrix() const {
    glm::mat4 Sxyz;
    Sxyz[0] = glm::vec4(1/(far_plane * tan(getWidthAngle()/2)), 0, 0, 0);
    Sxyz[1] = glm::vec4(0, 1/(far_plane * tan(getHeightAngle()/2)), 0, 0);
    Sxyz[2] = glm::vec4(0, 0, 1/far_plane, 0);
    Sxyz[3] = glm::vec4(0, 0, 0, 1);
    return Sxyz;
}

void Camera::updateCamaraSize(int w, int h) {
    camera_width = w;
    camera_height = h;
}

void Camera::updateNearPlane(double near) {
    near_plane = near;
}

void Camera::updateFarPlane(double far) {
    far_plane = far;
}
