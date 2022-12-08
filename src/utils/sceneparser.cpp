#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

// From Lab 5
void renderNodeHelper(SceneNode *n, glm::mat4 m, std::vector<RenderShapeData> &shapes) {
    glm::mat4 CTM = m;
    for (auto &t : n->transformations) {
        switch (t->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            CTM *= glm::translate(t->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            CTM *= glm::scale(t->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            CTM *= glm::rotate(t->angle, t->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            CTM *= t->matrix;
            break;
        }
    }
    for (auto &p : n->primitives) {
        shapes.push_back(RenderShapeData {*p, CTM});
    }
    for (auto &c : n->children) {
        renderNodeHelper(c, CTM, shapes);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // Populate renderData with global data, lights, and camera data
    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();
    renderData.cameraData = fileReader.getCameraData();

    // Populate renderData's list of primitives and their transforms
    SceneNode *root = fileReader.getRootNode();
    renderData.shapes.clear();
    renderNodeHelper(root, glm::mat4(1.0f), renderData.shapes);

    return true;
}
