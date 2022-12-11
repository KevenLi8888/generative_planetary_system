#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "GL/glew.h"

enum class IntersectLocation {
    SIDE, // Sphere, Cylinder, Cone
    TOP, // All shapes
    BOTTOM,
    FRONT, // Cube only, viewing direction: -z
    BACK,
    LEFT,
    RIGHT,
    NONE,
};

class Primitive {
public:
    virtual void updateParams(int param1, int param2) = 0;
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void makeVBO();
    void makeVAO();
    virtual glm::vec3 getUV(glm::vec3 pos, IntersectLocation intersect_location) = 0;
    GLuint getVBOId();
    GLuint getVAOId();
    std::vector<float> getVertexData();
    int getParam1();
    int getParam2();
    GLuint getVBOTangentId();

private:
    virtual void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) = 0;
    virtual void makePrimitive() = 0;
    void computeTangents();

protected:
    GLuint m_vbo;
    GLuint m_vao;
    std::vector<float> m_vertexData;
    int m_param1 = -1;
    int m_param2 = -1;
    float m_radius = 0.5;
    // Normal Mapping - Tangents and Bitangents
    GLuint m_vbo_tangent;
    std::vector<float> m_vertex_tangents;
};

class Sphere: public Primitive {
public:
    void updateParams(int param1, int param2);
    glm::vec3 getUV(glm::vec3 pos, IntersectLocation intersect_location = IntersectLocation::SIDE);

private:
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makePrimitive();
};

class Cylinder: public Primitive {
public:
    void updateParams(int param1, int param2);
    glm::vec3 getUV(glm::vec3 pos, IntersectLocation intersect_location);

private:
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right, IntersectLocation intersect_location);
    void makeSide(float currentTheta, float nextTheta);
    void makeCap(float currentTheta, float nextTheta);
    void makePrimitive();
};

class Cone: public Primitive {
public:
    void updateParams(int param1, int param2);
    glm::vec3 getUV(glm::vec3 pos, IntersectLocation intersect_location);

private:
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeCapTile(glm::vec3 top, glm::vec3 left, glm::vec3 right);
    void makeTipTile(glm::vec3 top, glm::vec3 left, glm::vec3 right, float currentTheta, float nextTheta);
    void makeSide(float currentTheta, float nextTheta);
    void makeCap(float currentTheta, float nextTheta);
    void makePrimitive();
};

class Cube: public Primitive {
public:
    void updateParams(int param1, int param2);
    glm::vec3 getUV(glm::vec3 pos, IntersectLocation intersect_location);

private:
    void makeConeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
                      IntersectLocation intersect_location);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  IntersectLocation intersect_location);
    void makePrimitive();
    // Not used
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    }
};