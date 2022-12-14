#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 1;
    float farPlane = 1;
    bool filter1 = false;
    bool filter2 = false;
    bool filter3 = false;
    bool filter4 = false;
    bool filter5 = false;
    bool filter6 = false;
    bool pause = false;
    bool orbitCamera = false;
    bool showOrbits = true;
    bool proceduralTexture = false;
    bool normalMapping = true;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
