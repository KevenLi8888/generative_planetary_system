#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // TODO: Use your code from Lab 5 here.
    // Task 4: populate renderData with global data, lights, and camera data
    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();
    renderData.cameraData = fileReader.getCameraData();

    // Task 5: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    auto root = fileReader.getRootNode();
    renderData.shapes.clear();
    traverse(root, glm::mat4(1.f), renderData);
    for (auto& primitive: renderData.shapes) {
        primitive.inversed_ctm = getInversedCTM(primitive.ctm);
    }
    return true;
}

void SceneParser::traverse(SceneNode *node, glm::mat4 parent_ctm, RenderData &renderData) {
    // NOTE: SceneNodes can have both transforms, primitives, and children in the same node.
    if (node == nullptr) {
        return;
    }

    auto node_ctm = calculateCTM(parent_ctm, node->transformations);

    if (!node->primitives.empty()) {
        for (auto& primitive: node->primitives) {
            // calculate ctm, append to renderData.shapes
            renderData.shapes.push_back(RenderShapeData {*primitive, node_ctm});
        }
    }
    for (auto child: node->children) {
        traverse(child, node_ctm, renderData);
    }
}

glm::mat4 SceneParser::calculateCTM(glm::mat4 parent_ctm, std::vector<SceneTransformation *> &transformations) {
    auto result = parent_ctm;
    for (auto& transformation: transformations) {
        switch (transformation->type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                result = result * glm::translate(transformation->translate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                result = result * glm::scale(transformation->scale);
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                result = result * glm::rotate(transformation->angle, transformation->rotate);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                result = result * transformation->matrix;
                break;
        }
    }
    return result;
}

glm::mat4 SceneParser::getInversedCTM(glm::mat4 ctm) {
    return glm::inverse(ctm);
}
