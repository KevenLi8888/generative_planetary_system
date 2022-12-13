// Part of the codes are from lab 7

#include "terraingenerator.h"

TerrainGenerator::TerrainGenerator() {
    // Define resolution of terrain generation
    m_resolution = 512;

    // Generate random vector lookup table
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
        m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

TerrainGenerator::~TerrainGenerator() {
    m_randVecLookup.clear();
}

void insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

std::vector<float>& TerrainGenerator::generateTerrainNormals() {
    for (int x = 0; x < m_resolution; ++x) {
        for(int y = 0; y < m_resolution; ++y) {
            auto n = getNormal(x, y);
            insertVec3(normals, n * 0.5f + 0.5f);
            normals.push_back(1.0);
        }
    }
    return normals;
}

std::vector<float>& TerrainGenerator::generateTerrainColors() {
    for (int x = 0; x < m_resolution; ++x) {
        for(int y = 0; y < m_resolution; ++y) {
            auto pos = getPosition(x, y);
            auto n = getNormal(x, y);
            auto color = getColor(n, pos);
            insertVec3(colors, color);
            colors.push_back(1.0);
        }
    }
    return colors;
}

std::vector<float> &TerrainGenerator::generateTerrainDisplacement() {
    return displacement;
}

glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col) {
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.
    // Use Y-up coordinate
    float x = 1.0 * row / m_resolution;
    float z = 1.0 * col / m_resolution;
    float y = getHeight(x, z);
    return glm::vec3(x,y,z);
}

float TerrainGenerator::getHeight(float x, float y) {
    // Task 6: modify this call to produce noise of a different frequency
    float z = 1.f/2 * computePerlin(x * 2, y * 2);

    // Task 7: combine multiple different octaves of noise to produce fractal perlin noise
    z += 1.f/4 * computePerlin(x * 4, y * 4);
    z += 1.f/8 * computePerlin(x * 8, y * 8);
    z += 1.f/16 * computePerlin(x * 16, y * 16);

    // Return 0 as placeholder
    return z;
}

glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    // Task 9: Compute the average normal for the given input indices
    // TODO: How to get neighbors' indices?

    // Reference: https://cs1230.graphics/labs/lab7-get-normals/
    // TA SOLUTION
    glm::vec3 normal = glm::vec3(0, 0, 0);
    std::vector<std::vector<int>> neighborOffsets = { // Counter-clockwise around the vertex
            {-1, -1},
            { 0, -1},
            { 1, -1},
            { 1,  0},
            { 1,  1},
            { 0,  1},
            {-1,  1},
            {-1,  0}
    };
    glm::vec3 V = getPosition(row,col);
    for (int i = 0; i < 8; ++i) {
        int n1RowOffset = neighborOffsets[i][0];
        int n1ColOffset = neighborOffsets[i][1];
        int n2RowOffset = neighborOffsets[(i + 1) % 8][0];
        int n2ColOffset = neighborOffsets[(i + 1) % 8][1];
        glm::vec3 n1 = getPosition(row + n1RowOffset, col + n1ColOffset);
        glm::vec3 n2 = getPosition(row + n2RowOffset, col + n2ColOffset);
        normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);
}

glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position
    glm::vec3 result;
    auto height = position.y;

    if (height >= 0.03) {
        result = {1, 1, 1};
    }
    else if (height >= 0.01) {
        result = {0.75, 0.82, 0.69};
    }
    else if (height >= -0.01) {
        result = {0.64, 0.74, 0.49};
    }
    else if (height >= -0.03) {
        result = {0.25, 0.49, 0.38};
    }
    else if (height >= -0.05) {
        result = {0.45, 0.66, 0.39};
    }
    else {
        result = {0.25, 0.38, 0.75};
    }

    // Return white as placeholder
    return result;
}

// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    auto ease = 3 * pow(alpha,2) - 2 * pow(alpha, 3);

    // Return 0 as placeholder
    return A + ease * (B - A);
}

float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int base_x = floor(x);
    int base_y = floor(y);

    // Task 2: compute offset vectors
    auto intersect = glm::vec2(x, y);
    auto offset_tl = intersect - glm::vec2(base_x, base_y);
    auto offset_tr = intersect - glm::vec2(base_x + 1, base_y);
    auto offset_bl = intersect - glm::vec2(base_x, base_y + 1);
    auto offset_br = intersect - glm::vec2(base_x + 1, base_y + 1);

    // Task 3: compute the dot product between offset and grid vectors
    auto dot_tl = glm::dot(sampleRandomVector(base_x, base_y), offset_tl);
    auto dot_tr = glm::dot(sampleRandomVector(base_x + 1, base_y), offset_tr);
    auto dot_bl = glm::dot(sampleRandomVector(base_x, base_y + 1), offset_bl);
    auto dot_br = glm::dot(sampleRandomVector(base_x + 1, base_y + 1), offset_br);

    // Task 5: use your interpolation function to produce the final value
    auto G = interpolate(dot_tl, dot_tr, x-base_x);
    auto H = interpolate(dot_bl, dot_br, x-base_x);

    // Return 0 as a placeholder
    return interpolate(G, H, y-base_y);
}

