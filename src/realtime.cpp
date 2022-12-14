#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

#include "utils/shaderloader.h"
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Delete the shader program
    glDeleteProgram(m_phong_shader);
    glDeleteProgram(m_texture_shader);
    glDeleteProgram(m_invert_shader);
    glDeleteProgram(m_grayscale_shader);
    glDeleteProgram(m_chromatic_shader);
    glDeleteProgram(m_sharpen_shader);
    glDeleteProgram(m_boxblur_shader);
    glDeleteProgram(m_emboss_shader);
    glDeleteProgram(m_planet_shader);
    glDeleteProgram(m_normal_map_shader);

    // Renderer recycles its own resource in its destructor
    this->doneCurrent();
}

void Realtime::resetConfig() {
    // Sync config with the current settings
    m_config = {
        settings.shapeParameter1,
        settings.shapeParameter2,
        settings.nearPlane,
        settings.farPlane,
        settings.orbitCamera,
        settings.numPlanet
    };
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Store the current configuration
    resetConfig();

    // Set up shaders
    m_phong_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/phong.vert",
                "resources/shaders/phong.frag"
    );

    m_texture_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/texture.frag"
    );

    m_invert_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/invert.frag"
    );

    m_grayscale_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/grayscale.frag"
    );

    m_chromatic_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/chromatic.frag"
    );

    m_sharpen_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/sharpen.frag"
    );

    m_boxblur_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/boxblur.frag"
    );

    m_emboss_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/texture.vert",
                "resources/shaders/emboss.frag"
    );

    m_planet_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/phong.vert",
                "resources/shaders/planet.frag"
    );

    m_normal_map_shader = ShaderLoader::createShaderProgram(
            "resources/shaders/normalmap.vert",
            "resources/shaders/normalmap.frag"
    );

    configurePixelShaders();

    int screen_w = size().width() * m_devicePixelRatio;
    int screen_h = size().height() * m_devicePixelRatio;
    configureKernelShaders(screen_w, screen_h);

    m_renderer.initialize(screen_w, screen_h);
    m_renderer.updateScene(size().width(), size().height());
}

void Realtime::configurePixelShaders() {
    glUseProgram(m_phong_shader);
    glUniform1i(glGetUniformLocation(m_phong_shader, "tex"), 0);

    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "tex"), 0);

    glUseProgram(m_invert_shader);
    glUniform1i(glGetUniformLocation(m_invert_shader, "tex"), 0);

    glUseProgram(m_grayscale_shader);
    glUniform1i(glGetUniformLocation(m_grayscale_shader, "tex"), 0);

    glUseProgram(m_chromatic_shader);
    glUniform1i(glGetUniformLocation(m_chromatic_shader, "tex"), 0);

    glUseProgram(m_planet_shader);
    glUniform1i(glGetUniformLocation(m_planet_shader, "tex"), 0);

    glUseProgram(m_normal_map_shader);
    glUniform1i(glGetUniformLocation(m_planet_shader, "tex"), 0);

    glUseProgram(0);
}

void Realtime::configureKernelShaders(int w, int h) {
    float h_step = 1.0 / (float)w;
    float v_step = 1.0 / (float)h;

    glUseProgram(m_sharpen_shader);
    glUniform1i(glGetUniformLocation(m_sharpen_shader, "tex"), 0);
    glUniform1f(glGetUniformLocation(m_sharpen_shader, "h_step"), h_step);
    glUniform1f(glGetUniformLocation(m_sharpen_shader, "v_step"), v_step);

    glUseProgram(m_boxblur_shader);
    glUniform1i(glGetUniformLocation(m_boxblur_shader, "tex"), 0);
    glUniform1f(glGetUniformLocation(m_boxblur_shader, "h_step"), h_step);
    glUniform1f(glGetUniformLocation(m_boxblur_shader, "v_step"), v_step);

    glUseProgram(m_emboss_shader);
    glUniform1i(glGetUniformLocation(m_emboss_shader, "tex"), 0);
    glUniform1f(glGetUniformLocation(m_emboss_shader, "h_step"), h_step);
    glUniform1f(glGetUniformLocation(m_emboss_shader, "v_step"), v_step);

    glUseProgram(0);
}

void Realtime::paintGL() {
    GLuint phong_shader;
    if (settings.normalMapping)
        phong_shader = m_normal_map_shader;
    else
        phong_shader = m_planet_shader;

    if (settings.filter1)
        m_renderer.render(phong_shader, m_invert_shader);
    else if (settings.filter2)
        m_renderer.render(phong_shader, m_chromatic_shader);
    else if (settings.filter3)
        m_renderer.render(phong_shader, m_grayscale_shader);
    else if (settings.filter4)
        m_renderer.render(phong_shader, m_boxblur_shader);
    else if (settings.filter5)
        m_renderer.render(phong_shader, m_sharpen_shader);
    else if (settings.filter6)
        m_renderer.render(phong_shader, m_emboss_shader);
    else
        m_renderer.render(phong_shader, m_texture_shader);
}

void Realtime::resizeGL(int w, int h) {
    // Screen dimensions
    int screen_w = size().width() * m_devicePixelRatio;
    int screen_h = size().height() * m_devicePixelRatio;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, screen_w, screen_h);

    // Update shader uniforms
    configureKernelShaders(screen_w, screen_h);

    // Update the camera's projection matrix
    m_renderer.resizeCanvas(screen_w, screen_h);
    m_renderer.updateCamera(w, h);
}

void Realtime::sceneChanged() {
    m_renderer.updateScene(size().width(), size().height());
    resetConfig();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!m_renderer.isReady()) return;

    // Determine what needs to be updated
    bool needUpdateCamera = settings.nearPlane != m_config.nearPlane ||
                            settings.farPlane != m_config.farPlane;
    bool needUpdateGeometry = settings.shapeParameter1 != m_config.shapeParameter1 ||
                              settings.shapeParameter2 != m_config.shapeParameter2;
    bool needReplaceCamera = settings.orbitCamera != m_config.orbitCamera;

    // Update if necessary
    if (needUpdateCamera) m_renderer.updateCamera(size().width(), size().height());
    if (needUpdateGeometry) m_renderer.updateGeometry();
    if (needReplaceCamera) m_renderer.replaceCamera(size().width(), size().height());

    resetConfig();
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_renderer.rotateCamera(deltaX/120.0, deltaY/120.0);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    m_renderer.moveCamera(m_keyMap, deltaTime * 5);
    m_renderer.switchCamera(m_keyMap, deltaTime);

    // Update planet positions
    if (!settings.pause) {
        m_renderer.updatePlanets(deltaTime);
    }

    update(); // asks for a PaintGL() call to occur
}
