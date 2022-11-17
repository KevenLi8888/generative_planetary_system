#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

#include "utils/shaderloader.h"
#include "utils/primitive.h"
#include "utils/debug.h"

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

    // Students: anything requiring OpenGL calls when the program exits should be done here
    GLuint vbos[4] = {sphere.getVBOId(), cylinder.getVBOId(), cone.getVBOId(), cube.getVBOId()};
    glDeleteBuffers(4, &vbos[0]);
    GLuint vaos[4] = {sphere.getVAOId(), cylinder.getVAOId(), cone.getVAOId(), cube.getVAOId()};
    glDeleteVertexArrays(4, &vaos[0]);
    GLuint vbos_hi[4] = {sphere_hi.getVBOId(), cylinder_hi.getVBOId(), cone_hi.getVBOId(), cube_hi.getVBOId()};
    glDeleteBuffers(4, &vbos_hi[0]);
    GLuint vaos_hi[4] = {sphere_hi.getVAOId(), cylinder_hi.getVAOId(), cone_hi.getVAOId(), cube_hi.getVAOId()};
    glDeleteVertexArrays(4, &vaos_hi[0]);
    GLuint vbos_low[4] = {sphere_low.getVBOId(), cylinder_low.getVBOId(), cone_low.getVBOId(), cube_low.getVBOId()};
    glDeleteBuffers(4, &vbos_low[0]);
    GLuint vaos_low[4] = {sphere_low.getVAOId(), cylinder_low.getVAOId(), cone_low.getVAOId(), cube_low.getVAOId()};
    glDeleteVertexArrays(4, &vaos_low[0]);
    glDeleteProgram(m_shader);
    this->doneCurrent();
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

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",
                                                 ":/resources/shaders/default.frag");
    initialized = true;
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    auto m_view = camera.getViewMatrix();
    auto m_proj = camera.getProjectionMatrix();

    for (auto& shape: metaData.shapes) {
        Primitive* current_shape;
        if (settings.extraCredit2) {
            current_shape = getCurrentShape(shape.primitive.type, getDetailLevelByDistance(shape));
        }
        else if (settings.extraCredit1) {
            current_shape = getCurrentShape(shape.primitive.type, getDetailLevelByNumberOfObjects(metaData));
        }
        else {
            current_shape = getCurrentShape(shape.primitive.type, Detail::DETAIL_MID);
        }
        glBindVertexArray(current_shape->getVAOId());

        // Vertex Shader
        auto m_model = shape.ctm;
        auto m_model_location = glGetUniformLocation(m_shader, "model_mat");
        glUniformMatrix4fv(m_model_location, 1, GL_FALSE, &m_model[0][0]);
        auto m_model_inversed_transposed = glm::mat3(glm::transpose(shape.inversed_ctm));
        auto m_model_inversed_transposed_location = glGetUniformLocation(m_shader, "inversed_transposed_model_mat");
        glUniformMatrix3fv(m_model_inversed_transposed_location, 1, GL_FALSE, &m_model_inversed_transposed[0][0]);
        auto m_view_location = glGetUniformLocation(m_shader, "view_mat");
        glUniformMatrix4fv(m_view_location, 1, GL_FALSE, &m_view[0][0]);
        auto m_proj_location = glGetUniformLocation(m_shader, "proj_mat");
        glUniformMatrix4fv(m_proj_location, 1, GL_FALSE, &m_proj[0][0]);

        // Fragment Shader
        // GlobalData
        auto ka_location = glGetUniformLocation(m_shader, "global.ka");
        glUniform1f(ka_location, metaData.globalData.ka);
        auto kd_location = glGetUniformLocation(m_shader, "global.kd");
        glUniform1f(kd_location, metaData.globalData.kd);
        auto ks_location = glGetUniformLocation(m_shader, "global.ks");
        glUniform1f(ks_location, metaData.globalData.ks);
        // ShapeData
        auto ambient_location = glGetUniformLocation(m_shader, "shape.cAmbient");
        glUniform4fv(ambient_location, 1, &shape.primitive.material.cAmbient[0]);
        auto diffuse_location = glGetUniformLocation(m_shader, "shape.cDiffuse");
        glUniform4fv(diffuse_location, 1, &shape.primitive.material.cDiffuse[0]);
        auto specular_location = glGetUniformLocation(m_shader, "shape.cSpecular");
        glUniform4fv(specular_location, 1, &shape.primitive.material.cSpecular[0]);
        auto shininess_location = glGetUniformLocation(m_shader, "shape.shininess");
        glUniform1f(shininess_location, shape.primitive.material.shininess);
        // LightData lights[8]
        for (int i = 0; i < 8; ++i) {
            auto color_name = "lights[" + std::to_string(i) + "].color";
            auto light_color_location = glGetUniformLocation(m_shader, &color_name[0]);
            auto dir_name = "lights[" + std::to_string(i) + "].dir";
            auto light_dir_location = glGetUniformLocation(m_shader, &dir_name[0]);
            if (i < metaData.lights.size()) {
                glUniform4fv(light_color_location, 1, &metaData.lights[i].color[0]);
                glUniform4fv(light_dir_location, 1, &metaData.lights[i].dir[0]);
            }
            else {
                glUniform4fv(light_color_location, 1, &glm::vec4(0.f)[0]);
                glUniform4fv(light_dir_location, 1, &glm::vec4(0.f)[0]);
            }
        }
        // Camera Position
        auto cam_pos = camera.getCameraPosition();
        auto cam_pos_location = glGetUniformLocation(m_shader, "cam_pos_world");
        glUniform4fv(cam_pos_location, 1, &cam_pos[0]);
        glDrawArrays(GL_TRIANGLES, 0, current_shape->getVertexData().size()/6);
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // update camera
    camera.updateCamaraSize(size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
}

void Realtime::sceneChanged() {
    auto scene_path = settings.sceneFilePath;
    bool success = SceneParser::parse(scene_path, metaData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << scene_path << "\"" << std::endl;
    }

    // instantiate camera
    camera = Camera(size().width() * m_devicePixelRatio,
                    size().height() * m_devicePixelRatio,
                    metaData.cameraData,
                    settings.nearPlane,
                    settings.farPlane);

    initializeShapes();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    makeCurrent();
    camera.updateNearPlane(settings.nearPlane);
    camera.updateFarPlane(settings.farPlane);
    if (initialized and (settings.shapeParameter1 != current_param_1 or settings.shapeParameter2 != current_param_2)) {
        initializeShapes();
        current_param_1 = settings.shapeParameter1;
        current_param_2 = settings.shapeParameter2;
    }
    else if (initialized and (settings.extraCredit1 != extra_credit_1_toggle or settings.extraCredit2 != extra_credit_2_toggle)) {
        initializeShapes();
        extra_credit_1_toggle = settings.extraCredit1;
        extra_credit_2_toggle = settings.extraCredit2;
    }
    update(); // asks for a PaintGL() call to occur
}

void Realtime::setDefaultState() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::initializeShapes() {
    sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    sphere.makeVBO();
    sphere.makeVAO();
    setDefaultState();
    cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cylinder.makeVBO();
    cylinder.makeVAO();
    setDefaultState();
    cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cone.makeVBO();
    cone.makeVAO();
    setDefaultState();
    cube.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cube.makeVBO();
    cube.makeVAO();
    setDefaultState();

    // Extra Credit: Adaptive level of detail (default: mid)
    // 1 - Varies by number of objects (reuse near, mid, far for different levels)
    // 2 - Varies by distance (near, mid, far)
    if (settings.extraCredit1 or settings.extraCredit2) {
        // near
        sphere_hi.updateParams(round(settings.shapeParameter1 * 3),
                               round(settings.shapeParameter2 * 3));
        sphere_hi.makeVBO();
        sphere_hi.makeVAO();
        setDefaultState();
        cylinder_hi.updateParams(round(settings.shapeParameter1 * 3),
                                 round(settings.shapeParameter2 * 3));
        cylinder_hi.makeVBO();
        cylinder_hi.makeVAO();
        setDefaultState();
        cone_hi.updateParams(round(settings.shapeParameter1 * 3),
                             round(settings.shapeParameter2 * 3));
        cone_hi.makeVBO();
        cone_hi.makeVAO();
        setDefaultState();
        cube_hi.updateParams(round(settings.shapeParameter1 * 3),
                             round(settings.shapeParameter2 * 3));
        cube_hi.makeVBO();
        cube_hi.makeVAO();
        setDefaultState();
        // far
        sphere_low.updateParams(round(settings.shapeParameter1 * 0.5),
                                round(settings.shapeParameter2 * 0.5));
        sphere_low.makeVBO();
        sphere_low.makeVAO();
        setDefaultState();
        cylinder_low.updateParams(round(settings.shapeParameter1 * 0.5),
                                  round(settings.shapeParameter2 * 0.5));
        cylinder_low.makeVBO();
        cylinder_low.makeVAO();
        setDefaultState();
        cone_low.updateParams(round(settings.shapeParameter1 * 0.5),
                              round(settings.shapeParameter2 * 0.5));
        cone_low.makeVBO();
        cone_low.makeVAO();
        setDefaultState();
        cube_low.updateParams(round(settings.shapeParameter1 * 0.5),
                              round(settings.shapeParameter2 * 0.5));
        cube_low.makeVBO();
        cube_low.makeVAO();
        setDefaultState();
    }
}

Detail Realtime::getDetailLevelByDistance(RenderShapeData &shape) {
    auto m_view = camera.getViewMatrix();
    auto m_sxyz = camera.getSxyzMatrix();
    // location in canonical perspective view volume
    auto location = m_sxyz * m_view * shape.ctm * glm::vec4(0, 0, 0, 1);
    auto distance = glm::distance(location, glm::vec4(0, 0, 0, 1));
    if (distance >= 0 and distance < 1.f/5.f) {
        return Detail::DETAIL_HI;
    }
    else if (distance >= 1.f/5.f and distance < 1.f/2.f) {
        return Detail::DETAIL_MID;
    }
    else {
        return Detail::DETAIL_LOW;
    }
}

Detail Realtime::getDetailLevelByNumberOfObjects(RenderData &data) {
    auto object_number = data.shapes.size();
    if (object_number < 50) {
        return Detail::DETAIL_HI;
    }
    else if (object_number >= 50 and object_number < 200) {
        return Detail::DETAIL_MID;
    }
    else {
        return Detail::DETAIL_LOW;
    }
}

Primitive* Realtime::getCurrentShape(PrimitiveType primitive_type, Detail detail) {
    Primitive* current_shape;
    switch (primitive_type) {
        case PrimitiveType::PRIMITIVE_SPHERE:
            switch (detail) {
                case Detail::DETAIL_HI:
                    current_shape = &sphere_hi;
                    break;
                case Detail::DETAIL_MID:
                    current_shape = &sphere;
                    break;
                case Detail::DETAIL_LOW:
                    current_shape = &sphere_low;
                    break;
            }
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            switch (detail) {
                case Detail::DETAIL_HI:
                    current_shape = &cylinder_hi;
                    break;
                case Detail::DETAIL_MID:
                    current_shape = &cylinder;
                    break;
                case Detail::DETAIL_LOW:
                    current_shape = &cylinder_low;
                    break;
            }
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            switch (detail) {
                case Detail::DETAIL_HI:
                    current_shape = &cone_hi;
                    break;
                case Detail::DETAIL_MID:
                    current_shape = &cone;
                    break;
                case Detail::DETAIL_LOW:
                    current_shape = &cone_low;
                    break;
            }
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            switch (detail) {
                case Detail::DETAIL_HI:
                    current_shape = &cube_hi;
                    break;
                case Detail::DETAIL_MID:
                    current_shape = &cube;
                    break;
                case Detail::DETAIL_LOW:
                    current_shape = &cube_low;
                    break;
            }
            break;
    }
    return current_shape;
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

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    update(); // asks for a PaintGL() call to occur
}
