#pragma once

#include "renderer/renderer.h"

//#include <GL/glew.h>
#include <glm/glm.hpp>

//#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    struct Config {
        int shapeParameter1;
        int shapeParameter2;
        float nearPlane;
        float farPlane;
        bool orbitCamera;
    };
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
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

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // Current Config (used to track state change)
    Config m_config;
    void resetConfig();

    // 3D Renderer
    Renderer m_renderer;

    // Shader Programs
    GLuint m_phong_shader;
    GLuint m_texture_shader;
    GLuint m_invert_shader;
    GLuint m_grayscale_shader;
    GLuint m_chromatic_shader;
    GLuint m_sharpen_shader;
    GLuint m_boxblur_shader;
    GLuint m_emboss_shader;

    // Final Project
    GLuint m_planet_shader;
    GLuint m_normal_map_shader;

    void configurePixelShaders();
    void configureKernelShaders(int w, int h);
};
