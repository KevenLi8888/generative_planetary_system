#ifndef PROJECTS_REALTIME_TERRAINGENERATOR_H
#define PROJECTS_REALTIME_TERRAINGENERATOR_H

#include <vector>
#include "glm/glm.hpp"
#include <iostream>


class TerrainGenerator {
public:
    TerrainGenerator();
    ~TerrainGenerator();
    int getResolution() { return m_resolution; };
    std::vector<float>& generateTerrainNormals();
    std::vector<float>& generateTerrainColors();
    std::vector<float>& generateTerrainDisplacement();

private:
    std::vector<glm::vec2> m_randVecLookup;
    int m_resolution;
    int m_lookupSize;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> displacement;

    // Samples the (infinite) random vector grid at (row, col)
    glm::vec2 sampleRandomVector(int row, int col);

    // Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
    // Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
    glm::vec3 getPosition(int row, int col);

    // Takes a normalized (x, y) position, in range [0,1)
    // Returns a height value, z, by sampling a noise function
    float getHeight(float x, float y);

    // Computes the normal of a vertex by averaging neighbors
    glm::vec3 getNormal(int row, int col);

    // Computes color of vertex using normal and, optionally, position
    glm::vec3 getColor(glm::vec3 normal, glm::vec3 position);

    // Computes the intensity of Perlin noise at some point
    float computePerlin(float x, float y);
};


#endif //PROJECTS_REALTIME_TERRAINGENERATOR_H
