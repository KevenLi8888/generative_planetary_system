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

    // prepare color palette
    // https://imgur.com/a/OCCq2gl
    // Sun
    planet_color_palette[0] = std::vector{glm::vec3(0.97, 0.43, 0.10),
                                          glm::vec3(0.95, 0.36, 0.14),
                                          glm::vec3(0.99, 0.73, 0.00),
                                          glm::vec3(1.00, 0.91, 0.55)};
    // Mercury
    planet_color_palette[1] = std::vector{glm::vec3(0.35, 0.35, 0.34),
                                          glm::vec3(0.75, 0.74, 0.74),
                                          glm::vec3(0.55, 0.54, 0.53),
                                          glm::vec3(0.96, 0.96, 0.97)};
    // Venus
    planet_color_palette[2] = std::vector{glm::vec3(0.75, 0.77, 0.78),
                                          glm::vec3(0.85, 0.69, 0.57),
                                          glm::vec3(0.96, 0.86, 0.77),
                                          glm::vec3(0.97, 0.99, 0.99)};
    // Earth
    planet_color_palette[3] = std::vector{glm::vec3(0.12, 0.22, 0.44),
                                          glm::vec3(0.85, 0.75, 0.64),
                                          glm::vec3(0.22, 0.44, 0.38),
                                          glm::vec3(0.95, 0.98, 0.97)};
    // Mars
    planet_color_palette[4] = std::vector{glm::vec3(0.55, 0.36, 0.29),
                                          glm::vec3(0.95, 0.48, 0.37),
                                          glm::vec3(0.76, 0.43, 0.36),
                                          glm::vec3(0.85, 0.74, 0.62)};
    // Jupiter
    planet_color_palette[5] = std::vector{glm::vec3(0.60, 0.45, 0.07),
                                          glm::vec3(0.75, 0.51, 0.22),
                                          glm::vec3(0.75, 0.69, 0.61),
                                          glm::vec3(0.65, 0.44, 0.36)};
    // Saturn
    planet_color_palette[6] = std::vector{glm::vec3(0.95, 0.81, 0.53),
                                          glm::vec3(0.85, 0.72, 0.47),
                                          glm::vec3(0.62, 0.58, 0.44),
                                          glm::vec3(0.75, 0.64, 0.50)};
    // Uranus
    planet_color_palette[7] = std::vector{glm::vec3(0.64, 0.80, 0.82),
                                          glm::vec3(0.66, 0.82, 0.84),
                                          glm::vec3(0.71, 0.86, 0.87),
                                          glm::vec3(0.82, 0.94, 0.94)};
    // Neptune
    planet_color_palette[8] = std::vector{glm::vec3(0.37, 0.38, 0.60),
                                          glm::vec3(0.40, 0.48, 0.65),
                                          glm::vec3(0.45, 0.58, 0.75),
                                          glm::vec3(0.47, 0.62, 0.75)};
    // Pluto
    planet_color_palette[9] = std::vector{glm::vec3(0.84, 0.70, 0.73),
                                          glm::vec3(0.66, 0.62, 0.59),
                                          glm::vec3(0.96, 0.92, 0.87),
                                          glm::vec3(0.25, 0.17, 0.09)};
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

std::vector<float>& TerrainGenerator::generateTerrainColors(int type) {
    colors.clear();
    for (int x = 0; x < m_resolution; ++x) {
        for(int y = 0; y < m_resolution * 2; ++y) {
            glm::vec3 color;
            if (type < 5) {
                glm::vec3 pos;
                if (y <= m_resolution - 1) {
                    pos = getPosition(x, y);
                }
                else {
                    pos = getPosition(x, ((m_resolution - 1) - y % (m_resolution)));
                }
                color = getColorFromPerlin(pos, type);
            }
            else {
                color = getColorForRing(x, y, type);
            }
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

glm::vec3 TerrainGenerator::getColorFromPerlin(glm::vec3 position, int type) {
    glm::vec3 result;
    auto height = position.y;
    float threshold[7] = {0.05, 0.04, 0.02, 0.01, -0.01, -0.015, -0.03};

    if (height >= threshold[0]) {
        result = planet_color_palette[type][3];
    }
    else if (height >= threshold[1]) {
        auto a = (height-threshold[1])/(threshold[0]-threshold[1]);
        result = glm::mix(planet_color_palette[type][2], planet_color_palette[type][3], a);
    }
    else if (height >= threshold[2]) {
        result = planet_color_palette[type][2];
    }
    else if (height >= threshold[3]) {
        auto a = (height-threshold[3])/(threshold[2]-threshold[3]);
        result = glm::mix(planet_color_palette[type][1], planet_color_palette[type][2], a);
    }
    else if (height >= threshold[4]) {
        result = planet_color_palette[type][1];
    }
    else if (height >= threshold[5]) {
        auto a = (height-threshold[5])/(threshold[4]-threshold[5]);
        result = glm::mix(planet_color_palette[type][0], planet_color_palette[type][1], a);
    }
    else {
        result = planet_color_palette[type][0];
    }

    // Return white as placeholder
    return result;
}

// Helper for computePerlin() and, possibly, getColorFromPerlin()
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

glm::vec3 TerrainGenerator::getColorForRing(int x, int y, int type) {
    auto t = getBezierCurve(0, 75 + x % 25, 0, float(y/2)/m_resolution);
//    std::cout << t << std::endl;
    x += t;
    if (x < m_resolution / 8) {
        return planet_color_palette[type][3];
    }
    else if (x < 2 * m_resolution / 8) {
        return planet_color_palette[type][2];
    }
    else if (x < 7 * m_resolution / 16) {
        return planet_color_palette[type][1];
    }
    else if (x < 5 * m_resolution / 8) {
        return planet_color_palette[type][0];
    }
    else if (x < 6 * m_resolution / 8) {
        return planet_color_palette[type][1];
    }
    else if (x < 7 * m_resolution / 8) {
        return planet_color_palette[type][2];
    }
    else {
        return planet_color_palette[type][3];
    }
}

float TerrainGenerator::getBezierCurve(float p0, float p1, float p2, float t) {
    return (1-t) * (1-t) * p0 + 2 * (1-t) * t * p1 + t*t * p2;
}

