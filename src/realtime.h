#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
#include "utils/camera.h"
#include "utils/primitive.h"
#include "settings.h"
#include "utils/terraingenerator.h"

enum class Detail {
    DETAIL_HI,
    DETAIL_MID,
    DETAIL_LOW
};

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();                                // Load and parse scenefile here
    void settingsChanged();


public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Project 5
    void setDefaultState();
    void initializeShapes();                            // Initialize VBO and VAO for 4 shapes
    void paintScene();
    Primitive* getCurrentShape(PrimitiveType primitive_type, Detail detail);
    Detail getDetailLevelByDistance(RenderShapeData& shape); // Extra Credit - Adaptive level of detail (distance)
    Detail getDetailLevelByNumberOfObjects(RenderData& data);

    // Project 6
    void initializeFullscreenQuad();
    void makeFBO();
    void paintPostProcess(GLuint texture);
    // Extra Credit - Texture Mapping
    void loadTextures();
    QImage loadImageFromFile(const std::string& file_path);
    // Extra Credit - Shadow Mapping
    void makeDepthMaps();
    void paintDepthMaps();

    // Final Project
    void generateNormalMap();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // Project 5
    RenderData metaData;
    GLuint m_shader;
    Camera camera = {1, 1, metaData.cameraData, 1, 1};
    Sphere sphere;
    Cylinder cylinder;
    Cone cone;
    Cube cube;
    bool initialized = false;
    int current_param_1 = settings.shapeParameter1;
    int current_param_2 = settings.shapeParameter2;
    // Extra Credit: Adaptive level of detail
    Sphere sphere_hi;
    Sphere sphere_low;
    Cylinder cylinder_hi;
    Cylinder cylinder_low;
    Cone cone_hi;
    Cone cone_low;
    Cube cube_hi;
    Cube cube_low;
    bool extra_credit_1_toggle = false;
    bool extra_credit_2_toggle = false;

    // Project 6
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_fbo;
    GLuint m_fbo_postprocess;
    GLuint m_fbo_renderbuffer;
    GLuint m_defaultFBO = 2;
    GLuint m_postprocess_shader;
    std::map<std::string, GLuint> scene_textures;
    // Extra Credit: Shadow mapping
    const int shadow_width = 4096, shadow_height = 4096;
    GLuint m_depth_fbos[8] = {0}; // directional lights only
    GLuint m_depth_maps[8] = {0}; // directional lights only
    int directional_light_count = 0;
    GLuint m_shadowmapping_shader;

    // Final Project
    GLuint m_normal_map;
//    GLuint m_color_map;
    TerrainGenerator m_terrain;
    int planet_type_count = 10;
};
