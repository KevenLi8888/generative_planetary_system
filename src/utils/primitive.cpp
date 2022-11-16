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
    Debug::glErrorCheck();
}

void Primitive::makeVAO() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    auto stride = sizeof(GL_FLOAT) * 6;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(0));
    auto normal_offset = sizeof(GL_FLOAT) * 3;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(normal_offset));
    Debug::glErrorCheck();
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
    auto n2 = glm::normalize(bottomLeft - origin);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n2);
    auto n3 = glm::normalize(bottomRight - origin);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n3);
    auto n4 = glm::normalize(bottomRight - origin);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n4);
    auto n5 = glm::normalize(topRight - origin);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n5);
    auto n6 = glm::normalize(topLeft - origin);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n6);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    auto stride = M_PI / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 current_top_left, current_top_right, current_bottom_left, current_bottom_right;
        auto r = m_radius;
        auto current_phi = i * stride;
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
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
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
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
}

void Cylinder::makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right) {
    auto n_top = glm::normalize(glm::cross(left-top, right-top));
    auto n_left = glm::normalize(glm::cross(right-left, top-left));
    auto n_right = glm::normalize(glm::cross(top-right, left-right));
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
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
    makeCapTile(top, top + stride_left, top + stride_right);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeCapTile(top + stride_left * float(i), top + stride_left * float(i + 1), top + stride_right * float(i));
        makeCapTile(top + stride_right * float(i), top + stride_left * float(i + 1), top + stride_right * float(i + 1));
    }
    // bottom cap
    // caution: vertices should be counter-clockwise
    top = glm::vec3(0, -0.5, 0);
    left = glm::vec3(r * sin(nextTheta), -0.5,r * cos(nextTheta));
    right = glm::vec3(r * sin(currentTheta), -0.5,r * cos(currentTheta));
    stride_left = (left - top) / float(m_param1);
    stride_right = (right - top) / float(m_param1);
    makeCapTile(top, top + stride_left, top + stride_right);
    for (int i = 1; i <= m_param1 - 1; ++i) {
        makeCapTile(top + stride_left * float(i), top + stride_left * float(i + 1), top + stride_right * float(i));
        makeCapTile(top + stride_right * float(i), top + stride_left * float(i + 1), top + stride_right * float(i + 1));
    }
}

void Cylinder::makePrimitive() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeSide(currentTheta, nextTheta);
        makeCap(currentTheta, nextTheta);
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
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n_right);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n_right);
}

void Cone::makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right) {
    auto n_top = glm::normalize(glm::cross(left-top, right-top));
    auto n_left = glm::normalize(glm::cross(right-left, top-left));
    auto n_right = glm::normalize(glm::cross(top-right, left-right));
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
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
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, n_top);
    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, n_left);
    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, n_right);
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
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeSide(currentTheta, nextTheta);
        makeCap(currentTheta, nextTheta);
    }
}

void Cube::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    makePrimitive();
}

void Cube::makeSideTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    // CAUTION: use right hand rule to determine the normal direction (use counter-clockwise order for cross product)
    auto n1 = glm::normalize(glm::cross(bottomLeft-topLeft, bottomRight-topLeft));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n1);
    auto n2 = glm::normalize(glm::cross(bottomRight-bottomLeft, topLeft-bottomLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n2);
    auto n3 = glm::normalize(glm::cross(topLeft-bottomRight, bottomLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n3);
    auto n4 = glm::normalize(glm::cross(topRight-bottomRight, topLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n4);
    auto n5 = glm::normalize(glm::cross(topLeft-topRight, bottomRight-topRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n5);
    auto n6 = glm::normalize(glm::cross(bottomRight-topLeft, topRight-topLeft));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n6);

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    auto stride_right = (topRight - topLeft) / float(m_param1);
    auto stride_down = (bottomLeft - topLeft) / float(m_param1);
    for (int i = 0; i < pow(m_param1, 2); ++i) {
        auto current_row = i / m_param1;
        auto current_col = i % m_param1;
        auto current_top_left = topLeft + float(current_col) * stride_right + float(current_row) * stride_down;
        auto current_top_right = current_top_left + stride_right;
        auto current_bottom_left = current_top_left + stride_down;
        auto current_bottom_right = current_top_left + stride_right + stride_down;
        makeSideTile(current_top_left, current_top_right, current_bottom_left, current_bottom_right);
    }
}

void Cube::makePrimitive() {
    // FRONT
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));
    // BACK
    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3( -0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3( -0.5f, -0.5f, -0.5f));
    // TOP
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f));
    // BOTTOM
    makeFace(glm::vec3(-0.5f,  -0.5f, 0.5f),
             glm::vec3( 0.5f,  -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
    // LEFT
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( -0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( -0.5f, -0.5f, 0.5f));
    // RIGHT
    makeFace(glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
}
