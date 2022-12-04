#include "primitive.h"
#include "utils/debug.h"


void Primitive::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Primitive::makeVBO() {
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    makePrimitive();
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(GLfloat),
                 m_vertexData.data(), GL_STATIC_DRAW);
}

void Primitive::makeVAO() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    auto stride = sizeof(GL_FLOAT) * 9;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(0));
    auto normal_offset = sizeof(GL_FLOAT) * 3;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(normal_offset));
    auto uv_offset = sizeof(GL_FLOAT) * 6;
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(uv_offset));
}

GLuint Primitive::getVBOId() {
    return m_vbo;
}

GLuint Primitive::getVAOId() {
    return m_vao;
}

std::vector<float> Primitive::getVertexData() {
    return m_vertexData;
}

int Primitive::getParam1() {
    return m_param1;
}

int Primitive::getParam2() {
    return m_param2;
}

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = (param1 < 2) ? 2 : param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    makePrimitive();
}

void Sphere::makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    auto origin = glm::vec3(0, 0, 0);
    auto n1 = glm::normalize(topLeft - origin);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n1);
    insertVec3(m_vertexData, getUV(topLeft));
    auto n2 = glm::normalize(bottomLeft - origin);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n2);
    insertVec3(m_vertexData, getUV(bottomLeft));
    auto n3 = glm::normalize(bottomRight - origin);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n3);
    insertVec3(m_vertexData, getUV(bottomRight));
    auto n4 = glm::normalize(bottomRight - origin);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n4);
    insertVec3(m_vertexData, getUV(bottomRight));
    auto n5 = glm::normalize(topRight - origin);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n5);
    insertVec3(m_vertexData, getUV(topRight));
    auto n6 = glm::normalize(topLeft - origin);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n6);
    insertVec3(m_vertexData, getUV(topLeft));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    auto stride = M_PI / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 current_top_left, current_top_right, current_bottom_left, current_bottom_right;
        auto r = m_radius;
        auto current_phi = i * stride;
        // for OpenGL to correctly handle uv interpolation
        if (current_phi == 0) {
            current_phi += 0.0001;
        }
        else if (abs(current_phi + stride - M_PI) <= 0.0001) {
            // takes float precision into consideration
            current_phi -= 0.0001;
        }
        current_top_left = {r * sin(current_phi) * sin(currentTheta),
                            r * cos(current_phi),
                            r * sin(current_phi) * cos(currentTheta)};
        current_top_right = {r * sin(current_phi) * sin(nextTheta),
                             r * cos(current_phi),
                             r * sin(current_phi) * cos(nextTheta)};
        current_bottom_left = {r * sin(current_phi + stride) * sin(currentTheta),
                               r * cos(current_phi + stride),
                               r * sin(current_phi + stride) * cos(currentTheta)};
        current_bottom_right = {r * sin(current_phi + stride) * sin(nextTheta),
                                r * cos(current_phi + stride),
                                r * sin(current_phi + stride) * cos(nextTheta)};
        makeSideTile(current_top_left, current_top_right, current_bottom_left, current_bottom_right);
    }
}

void Sphere::makePrimitive() {
    // start at pi/2
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep + M_PI/2;
        float nextTheta = (i+1) * thetaStep + M_PI/2;
        // for OpenGL to correctly handle uv interpolation
        // in makePrimitive, theta starts from (x, z) = (0, 1)
        // but in getUV, u = 0 or 1 when (x, z) = (1, 0)
        // so for edge cases, theta is not 0 or 2pi, it should be pi/2
        // if starts at 0, it's hard to handle the edge case (pi/2 point might be in the middle of a step)
        // so starts at pi/2
        if (currentTheta == 0) {
            currentTheta += 0.0001;
        }
        if (abs(nextTheta - (2*M_PI + M_PI/2)) <= 0.0001) {
            // takes float precision into consideration
            nextTheta -= 0.0001;
        }
        makeWedge(currentTheta, nextTheta);
    }
}

glm::vec3 Sphere::getUV(glm::vec3 pos, IntersectLocation intersect_location) {
    auto theta = atan2(pos.z, pos.x);
    auto u = (theta < 0) ? -theta/(2*M_PI) : 1-theta/(2*M_PI);
    auto r = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2));
    auto phi = asin(pos.y / r);
    auto v = phi/M_PI + 0.5;
    // modified for OpenGL to correctly interpolates uv
    // u cannot be a set value of 0.5 when v == 0 or v == 1
    // handled when calling makeWedge(), v will never equal to 0 or 1
    return {u, v, 1.f};
}

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    makePrimitive();
}

void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    auto n_left = glm::normalize(glm::vec3(2*topLeft.x, 0, 2*topLeft.z));
    auto n_right = glm::normalize(glm::vec3(2*topRight.x, 0, 2*topRight.z));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(topRight, IntersectLocation::SIDE));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(topLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(bottomLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(bottomLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(bottomRight, IntersectLocation::SIDE));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(topRight, IntersectLocation::SIDE));
}

void Cylinder::makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right, IntersectLocation intersect_location) {
    auto n_top = glm::normalize(glm::cross(left-top, right-top));
    auto n_left = glm::normalize(glm::cross(right-left, top-left));
    auto n_right = glm::normalize(glm::cross(top-right, left-right));
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, getUV(top, intersect_location));
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(left, intersect_location));
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(right, intersect_location));
}

void Cylinder::makeSide(float currentTheta, float nextTheta) {
    auto stride = float(1) / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 current_top_left, current_top_right, current_bottom_left, current_bottom_right;
        auto r = m_radius;
        current_top_left = {r * sin(currentTheta),
                            0.5 - stride * i,
                            r * cos(currentTheta)};
        current_top_right = {r * sin(nextTheta),
                             0.5 - stride * i,
                             r * cos(nextTheta)};
        current_bottom_left = {r * sin(currentTheta),
                               0.5 - stride * (i + 1),
                               r * cos(currentTheta)};
        current_bottom_right = {r * sin(nextTheta),
                                0.5 - stride * (i + 1),
                                r * cos(nextTheta)};
        makeSideTile(current_top_left, current_top_right, current_bottom_left, current_bottom_right);
    }
}

void Cylinder::makeCap(float currentTheta, float nextTheta) {
    auto r = m_radius;
    // top cap
    auto top = glm::vec3(0, 0.5, 0);
    auto left = glm::vec3(r * sin(currentTheta), 0.5,r * cos(currentTheta));
    auto right = glm::vec3(r * sin(nextTheta), 0.5,r * cos(nextTheta));
    auto stride_left = (left - top) / float(m_param1);
    auto stride_right = (right - top) / float(m_param1);
    makeCapTile(top, top + stride_left, top + stride_right, IntersectLocation::TOP);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeCapTile(top + stride_left * float(i), top + stride_left * float(i + 1), top + stride_right * float(i), IntersectLocation::TOP);
        makeCapTile(top + stride_right * float(i), top + stride_left * float(i + 1), top + stride_right * float(i + 1), IntersectLocation::TOP);
    }
    // bottom cap
    // caution: vertices should be counter-clockwise
    top = glm::vec3(0, -0.5, 0);
    left = glm::vec3(r * sin(nextTheta), -0.5,r * cos(nextTheta));
    right = glm::vec3(r * sin(currentTheta), -0.5,r * cos(currentTheta));
    stride_left = (left - top) / float(m_param1);
    stride_right = (right - top) / float(m_param1);
    makeCapTile(top, top + stride_left, top + stride_right, IntersectLocation::BOTTOM);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeCapTile(top + stride_left * float(i), top + stride_left * float(i + 1), top + stride_right * float(i), IntersectLocation::BOTTOM);
        makeCapTile(top + stride_right * float(i), top + stride_left * float(i + 1), top + stride_right * float(i + 1), IntersectLocation::BOTTOM);
    }
}

void Cylinder::makePrimitive() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep + M_PI/2;
        float nextTheta = (i+1) * thetaStep + M_PI/2;
        if (currentTheta == 0) {
            currentTheta += 0.0001;
        }
        if (abs(nextTheta - (2*M_PI + M_PI/2)) <= 0.0001) {
            // takes float precision into consideration
            nextTheta -= 0.0001;
        }
        makeSide(currentTheta, nextTheta);
        makeCap(currentTheta, nextTheta);
    }
}

glm::vec3 Cylinder::getUV(glm::vec3 pos, IntersectLocation intersect_location) {
    if (intersect_location == IntersectLocation::TOP) {
        auto u = pos.x + 0.5;
        auto v = - pos.z + 0.5;
        return {u, v, 1.f};
    }
    else if (intersect_location == IntersectLocation::BOTTOM) {
        auto u = pos.x + 0.5;
        auto v = pos.z + 0.5;
        return {u, v, 1.f};
    }
    else {
        auto theta = atan2(pos.z, pos.x);
        auto u = (theta < 0) ? -theta/(2*M_PI) : 1-theta/(2*M_PI);
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
}

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    makePrimitive();
}

void Cone::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {
    auto n_left = glm::normalize(glm::vec3(2*topLeft.x, -0.5*topLeft.y+0.25, 2*topLeft.z));
    auto n_right = glm::normalize(glm::vec3(2*topRight.x, -0.5*topRight.y+0.25, 2*topRight.z));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(topRight, IntersectLocation::SIDE));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(topLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(bottomLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(bottomLeft, IntersectLocation::SIDE));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(bottomRight, IntersectLocation::SIDE));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(topRight, IntersectLocation::SIDE));
}

void Cone::makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right) {
    auto n_top = glm::normalize(glm::cross(left-top, right-top));
    auto n_left = glm::normalize(glm::cross(right-left, top-left));
    auto n_right = glm::normalize(glm::cross(top-right, left-right));
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, getUV(top, IntersectLocation::BOTTOM));
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(left, IntersectLocation::BOTTOM));
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(right, IntersectLocation::BOTTOM));
}

void Cone::makeTipTile(glm::vec3 top, glm::vec3 left, glm::vec3 right, float currentTheta, float nextTheta) {
    auto step = (nextTheta - currentTheta) / 2;
    auto r = m_radius;
    auto btm_mid_point = glm::vec3(r * sin(currentTheta + step),
                                   -0.5,
                                   r * cos(currentTheta + step));
    auto n_top = glm::normalize(glm::vec3(2*btm_mid_point.x, -0.5*btm_mid_point.y+0.25, 2*btm_mid_point.z));;
    auto n_left = glm::normalize(glm::vec3(2*left.x, -0.5*left.y+0.25, 2*left.z));
    auto n_right = glm::normalize(glm::vec3(2*right.x, -0.5*right.y+0.25, 2*right.z));
    // modified for OpenGL to correctly interpolates uv
    top = top + glm::normalize(((left - top) + (right - top))) * float(0.0001);
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, getUV(top, IntersectLocation::SIDE));
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, getUV(left, IntersectLocation::SIDE));
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, getUV(right, IntersectLocation::SIDE));
}

void Cone::makeSide(float currentTheta, float nextTheta) {
    auto r = m_radius;
    auto top = glm::vec3(0, 0.5, 0);
    auto left = glm::vec3(r * sin(currentTheta), -0.5,r * cos(currentTheta));
    auto right = glm::vec3(r * sin(nextTheta), -0.5,r * cos(nextTheta));
    auto stride_left = (left - top) / float(m_param1);
    auto stride_right = (right - top) / float(m_param1);
    makeTipTile(top, top + stride_left, top + stride_right, currentTheta, nextTheta);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeSideTile(top + stride_left * float(i),
                     top + stride_right * float(i),
                     top + stride_left * float(i + 1),
                     top + stride_right * float(i + 1));
    }
}

void Cone::makeCap(float currentTheta, float nextTheta) {
    auto r = m_radius;
    auto top = glm::vec3(0, -0.5, 0);
    auto left = glm::vec3(r * sin(nextTheta), -0.5,r * cos(nextTheta));
    auto right = glm::vec3(r * sin(currentTheta), -0.5,r * cos(currentTheta));
    auto stride_left = (left - top) / float(m_param1);
    auto stride_right = (right - top) / float(m_param1);
    makeCapTile(top, top + stride_left, top + stride_right);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeCapTile(top + stride_left * float(i), top + stride_left * float(i + 1), top + stride_right * float(i));
        makeCapTile(top + stride_right * float(i), top + stride_left * float(i + 1), top + stride_right * float(i + 1));
    }
}

void Cone::makePrimitive() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep + M_PI/2;
        float nextTheta = (i+1) * thetaStep + M_PI/2;
        if (currentTheta == 0) {
            currentTheta += 0.0001;
        }
        if (abs(nextTheta - (2*M_PI + M_PI/2)) <= 0.0001) {
            // takes float precision into consideration
            nextTheta -= 0.0001;
        }
        makeSide(currentTheta, nextTheta);
        makeCap(currentTheta, nextTheta);
    }
}

glm::vec3 Cone::getUV(glm::vec3 pos, IntersectLocation intersect_location) {
    if (intersect_location == IntersectLocation::BOTTOM) {
        auto u = pos.x + 0.5;
        auto v = pos.z + 0.5;
        return {u, v, 1.f};
    }
    else {
        auto theta = atan2(pos.z, pos.x);
        auto u = (theta < 0) ? -theta/(2*M_PI) : 1-theta/(2*M_PI);
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
}

void Cube::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    makePrimitive();
}

void Cube::makeConeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        IntersectLocation intersect_location) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    // CAUTION: use right hand rule to determine the normal direction (use counter-clockwise order for cross product)
    auto n1 = glm::normalize(glm::cross(bottomLeft-topLeft, bottomRight-topLeft));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n1);
    insertVec3(m_vertexData, getUV(topLeft, intersect_location));
    auto n2 = glm::normalize(glm::cross(bottomRight-bottomLeft, topLeft-bottomLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n2);
    insertVec3(m_vertexData, getUV(bottomLeft, intersect_location));
    auto n3 = glm::normalize(glm::cross(topLeft-bottomRight, bottomLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n3);
    insertVec3(m_vertexData, getUV(bottomRight, intersect_location));
    auto n4 = glm::normalize(glm::cross(topRight-bottomRight, topLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n4);
    insertVec3(m_vertexData, getUV(bottomRight, intersect_location));
    auto n5 = glm::normalize(glm::cross(topLeft-topRight, bottomRight-topRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n5);
    insertVec3(m_vertexData, getUV(topRight, intersect_location));
    auto n6 = glm::normalize(glm::cross(bottomRight-topLeft, topRight-topLeft));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n6);
    insertVec3(m_vertexData, getUV(topLeft, intersect_location));

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    IntersectLocation intersect_location) {
    auto stride_right = (topRight - topLeft) / float(m_param1);
    auto stride_down = (bottomLeft - topLeft) / float(m_param1);
    for (int i = 0; i < pow(m_param1, 2); ++i) {
        auto current_row = i / m_param1;
        auto current_col = i % m_param1;
        auto current_top_left = topLeft + float(current_col) * stride_right + float(current_row) * stride_down;
        auto current_top_right = current_top_left + stride_right;
        auto current_bottom_left = current_top_left + stride_down;
        auto current_bottom_right = current_top_left + stride_right + stride_down;
        makeConeSideTile(current_top_left, current_top_right, current_bottom_left, current_bottom_right, intersect_location);
    }
}

void Cube::makePrimitive() {
    // FRONT
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             IntersectLocation::FRONT);
    // BACK
    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3( -0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3( -0.5f, -0.5f, -0.5f),
             IntersectLocation::BACK);
    // TOP
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f),
             IntersectLocation::TOP);
    // BOTTOM
    makeFace(glm::vec3(-0.5f,  -0.5f, 0.5f),
             glm::vec3( 0.5f,  -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             IntersectLocation::BOTTOM);
    // LEFT
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( -0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( -0.5f, -0.5f, 0.5f),
             IntersectLocation::LEFT);
    // RIGHT
    makeFace(glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             IntersectLocation::RIGHT);
}

glm::vec3 Cube::getUV(glm::vec3 pos, IntersectLocation intersect_location) {
    if (intersect_location == IntersectLocation::TOP) {
        auto u = pos.x + 0.5;
        auto v = - pos.z + 0.5;
        return {u, v, 1.f};
    }
    else if (intersect_location == IntersectLocation::BOTTOM) {
        auto u = pos.x + 0.5;
        auto v = pos.z + 0.5;
        return {u, v, 1.f};
    }
    else if (intersect_location == IntersectLocation::FRONT) {
        auto u = pos.x + 0.5;
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
    else if (intersect_location == IntersectLocation::BACK) {
        auto u = - pos.x + 0.5;
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
    else if (intersect_location == IntersectLocation::LEFT) {
        auto u = pos.z + 0.5;
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
    else {
        auto u = - pos.z + 0.5;
        auto v = pos.y + 0.5;
        return {u, v, 1.f};
    }
}
