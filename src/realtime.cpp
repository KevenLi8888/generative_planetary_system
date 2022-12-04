#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <QOpenGLFramebufferObject>
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
    glDeleteProgram(m_postprocess_shader);
    glDeleteProgram(m_shadowmapping_shader);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteTextures(1, &m_fbo_postprocess);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    for (int i = 0; i < directional_light_count; ++i) {
        glDeleteTextures(1, &m_depth_maps[i]);
        glDeleteFramebuffers(1, &m_depth_fbos[i]);
    }
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
    // Load shaders
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",
                                                 ":/resources/shaders/default.frag");

    // TODO: separate codes for post-processing
    m_postprocess_shader = ShaderLoader::createShaderProgram(":/resources/shaders/postprocessing.vert",
                                                             ":/resources/shaders/postprocessing.frag");
    initializeFullscreenQuad();
    makeFBO();

    m_shadowmapping_shader = ShaderLoader::createShaderProgram(":/resources/shaders/shadowmapping.vert",
                                                               ":/resources/shaders/shadowmapping.frag");
    initialized = true;
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here

    // Shadow Mapping
    paintDepthMaps();

    // Task 24: Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 28: Call glViewport
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintScene();

    // Task 25: Bind the default framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    QOpenGLFramebufferObject::bindDefault();

    // Task 28: Call glViewport
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Task 26: Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Task 27: Call paintTexture to draw our FBO color attachment texture | Task 31: Set bool parameter to true
    paintPostProcess(m_fbo_postprocess);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // Task 34: Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_postprocess);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    makeFBO();
    // update camera
    camera.updateCamaraSize(size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
}

void Realtime::sceneChanged() {
    auto scene_path = settings.sceneFilePath;
    bool success = SceneParser::parse(scene_path, metaData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << scene_path << "\"" << std::endl;
    }

    loadTextures();

    // instantiate camera
    camera = Camera(size().width() * m_devicePixelRatio,
                    size().height() * m_devicePixelRatio,
                    metaData.cameraData,
                    settings.nearPlane,
                    settings.farPlane);

    initializeShapes();

    // Shadow Mapping
    makeDepthMaps();

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

void Realtime::paintScene() {
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

        // Texture Mapping - Project 6 Extra Credit
        auto blend = (settings.extraCredit3) ? shape.primitive.material.blend : 0;
        auto m_texture = (settings.extraCredit3 and shape.primitive.material.textureMap.isUsed) ?
                scene_textures.at(shape.primitive.material.textureMap.filename) : 0;
        auto texture_sampler_pos = glGetUniformLocation(m_shader, "texture_sampler");
        glUniform1i(texture_sampler_pos, 0);
        auto blend_pos = glGetUniformLocation(m_shader, "blend");
        glUniform1f(blend_pos, blend);
        auto hor_repeat_pos = glGetUniformLocation(m_shader, "hor_repeat");
        glUniform1f(hor_repeat_pos, shape.primitive.material.textureMap.repeatU);
        auto vert_repeat_pos = glGetUniformLocation(m_shader, "vert_repeat");
        glUniform1f(vert_repeat_pos, shape.primitive.material.textureMap.repeatV);

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
            auto type_name = "lights[" + std::to_string(i) + "].type";
            auto light_type_location = glGetUniformLocation(m_shader, &type_name[0]);
            auto color_name = "lights[" + std::to_string(i) + "].color";
            auto light_color_location = glGetUniformLocation(m_shader, &color_name[0]);
            auto function_name = "lights[" + std::to_string(i) + "].function";
            auto light_function_location = glGetUniformLocation(m_shader, &function_name[0]);
            auto dir_name = "lights[" + std::to_string(i) + "].dir";
            auto light_dir_location = glGetUniformLocation(m_shader, &dir_name[0]);
            auto pos_name = "lights[" + std::to_string(i) + "].pos";
            auto light_pos_location = glGetUniformLocation(m_shader, &pos_name[0]);
            auto penumbra_name = "lights[" + std::to_string(i) + "].penumbra";
            auto penumbra_location = glGetUniformLocation(m_shader, &penumbra_name[0]);
            auto angle_name = "lights[" + std::to_string(i) + "].angle";
            auto angle_location = glGetUniformLocation(m_shader, &angle_name[0]);
            auto light_space_mat_name = "light_space_mat[" + std::to_string(i) + "]";
            if (i < metaData.lights.size()) {
                switch (metaData.lights[i].type) {
                    case LightType::LIGHT_DIRECTIONAL:
                        glUniform1i(light_type_location, 0);
                        break;
                    case LightType::LIGHT_POINT:
                        glUniform1i(light_type_location, 1);
                        break;
                    case LightType::LIGHT_SPOT:
                        glUniform1i(light_type_location, 2);
                        break;
                    default:
                        glUniform1i(light_type_location, -1);
                        break;
                }
                glUniform4fv(light_color_location, 1, &metaData.lights[i].color[0]);
                glUniform3fv(light_function_location, 1, &metaData.lights[i].function[0]);
                glUniform4fv(light_dir_location, 1, &metaData.lights[i].dir[0]);
                glUniform4fv(light_pos_location, 1, &metaData.lights[i].pos[0]);
                glUniform1f(penumbra_location, metaData.lights[i].penumbra);
                glUniform1f(angle_location, metaData.lights[i].angle);

                // Shadow Mapping
                if (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL) {
                    auto light_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f,
                                                       settings.nearPlane, settings.farPlane);
                    auto light_inv_dir = glm::vec3(-metaData.lights[i].dir);
                    auto light_view = glm::lookAt(light_inv_dir,
                                                  glm::vec3(0.f, 0.f, 0.f),
                                                  glm::vec3(0.f, 1.f, 0.f));
                    auto light_space_matrix = light_projection * light_view;
                    glUniformMatrix4fv(glGetUniformLocation(m_shader, &light_space_mat_name[0]),
                                       1, GL_FALSE, &light_space_matrix[0][0]);
                }
                else {
                    auto light_space_matrix = glm::mat4(1.0);
                    glUniformMatrix4fv(glGetUniformLocation(m_shader, &light_space_mat_name[0]),
                                       1, GL_FALSE, &light_space_matrix[0][0]);
                }
            }
            else {
                glUniform1i(light_type_location, -1);
                auto light_space_matrix = glm::mat4(1.0);
                glUniformMatrix4fv(glGetUniformLocation(m_shader, &light_space_mat_name[0]),
                                   1, GL_FALSE, &light_space_matrix[0][0]);
            }
        }

        // Camera Position
        auto cam_pos = camera.getCameraPosition();
        auto cam_pos_location = glGetUniformLocation(m_shader, "cam_pos_world");
        glUniform4fv(cam_pos_location, 1, &cam_pos[0]);

        // Extra Credit - Texture Mapping
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // Extra Credit - Shadow Mapping
        glUniform1i(glGetUniformLocation(m_shader, "enable_shadow_mapping"), settings.extraCredit4);
        for (int i = 0; i < 8; ++i) {
            auto shadow_map_pos_name = "shadow_map[" + std::to_string(i) + "]";
            auto shadow_map_pos = glGetUniformLocation(m_shader, &shadow_map_pos_name[0]);
            glUniform1i(shadow_map_pos, i+1);
        }
        // To support up to 8 directional lights... it's dirty code but it works
        // Using sample2DArray should be a good idea
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[0]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[1]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[2]);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[3]);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[4]);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[5]);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[6]);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[7]);

        glDrawArrays(GL_TRIANGLES, 0, current_shape->getVertexData().size()/9);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
    glUseProgram(0);
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
        float degree_x = 0.05 * deltaX;
        auto new_cam_look = glm::normalize(camera.getRotationMatrix(degree_x, {0,1,0}) * glm::vec3(camera.getCameraLook()));
        camera.updateCameraLook({new_cam_look, 1.f});
        float degree_y = 0.05 * deltaY;
        new_cam_look = glm::normalize(camera.getRotationMatrix(degree_y, glm::cross(glm::vec3(camera.getCameraLook()), glm::vec3(camera.getCameraUp())))
                * glm::vec3(camera.getCameraLook()));
        camera.updateCameraLook({new_cam_look, 1.f});
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    auto displacement = deltaTime * 5;
    if (m_keyMap[Qt::Key_W]) {
        auto new_pos = camera.getCameraPosition()
                + displacement * glm::normalize(camera.getCameraLook());
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
    if (m_keyMap[Qt::Key_S]) {
        auto new_pos = camera.getCameraPosition()
                - displacement * glm::normalize(camera.getCameraLook());
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
    if (m_keyMap[Qt::Key_A]) {
        auto new_pos = camera.getCameraPosition()
                - displacement * glm::normalize(glm::vec4(glm::cross(glm::vec3(camera.getCameraLook()), glm::vec3(camera.getCameraUp())),0));
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
    if (m_keyMap[Qt::Key_D]) {
        auto new_pos = camera.getCameraPosition()
                       + displacement * glm::normalize(glm::vec4(glm::cross(glm::vec3(camera.getCameraLook()), glm::vec3(camera.getCameraUp())),0));
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
    if (m_keyMap[Qt::Key_Space]) {
        auto new_pos = camera.getCameraPosition()
                       + displacement * glm::vec4(0, 1, 0, 0);
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
    if (m_keyMap[Qt::Key_Control]) {
        auto new_pos = camera.getCameraPosition()
                       + displacement * glm::vec4(0, -1, 0, 0);
        camera.updateCameraPosition(new_pos);
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::initializeFullscreenQuad() {
    // Task 10: Set the texture.frag uniform for our texture
    glUseProgram(m_postprocess_shader);
    auto postprocess_sampler_pos = glGetUniformLocation(m_postprocess_shader, "postprocess_sampler");
    glUniform1i(postprocess_sampler_pos, 0);

    // Task 11: Fix this "fullscreen" quad's vertex data
    // Task 12: Play around with different values!
    // Task 13: Add UV coordinates
    std::vector<GLfloat> fullscreen_quad_data = {
            //     POSITIONS    //
            -1.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,

            -1.0f, -1.0f, 0.0f,
             0.0f, 0.0f, 0.0f,

             1.0f, -1.0f, 0.0f,
             1.0f, 0.0f, 0.0f,

             1.0f, 1.0f, 0.0f,
             1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,

             1.0f, -1.0f, 0.0f,
             1.0f, 0.0f, 0.0f
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));


    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::makeFBO() {
    // Codes ported from lab 11
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_postprocess);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_postprocess);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_postprocess, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    QOpenGLFramebufferObject::bindDefault();
}

void Realtime::paintPostProcess(GLuint texture) {
    glUseProgram(m_postprocess_shader);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    glUniform1i(glGetUniformLocation(m_postprocess_shader, "grayscale"), settings.perPixelFilter);
    glUniform1i(glGetUniformLocation(m_postprocess_shader, "invert"), settings.perPixelFilter2);
    glUniform1i(glGetUniformLocation(m_postprocess_shader, "blur"), settings.kernelBasedFilter);
    glUniform1i(glGetUniformLocation(m_postprocess_shader, "sharpen"), settings.kernelBasedFilter2);
    glUniform1f(glGetUniformLocation(m_postprocess_shader, "width"), size().width() * m_devicePixelRatio);
    glUniform1f(glGetUniformLocation(m_postprocess_shader, "height"), size().height() * m_devicePixelRatio);

    glBindVertexArray(m_fullscreen_vao);
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::loadTextures() {
    for (auto& texture : scene_textures) {
        glDeleteTextures(1, &texture.second);
    }
    scene_textures.clear();
    for (auto& shape: metaData.shapes) {
        if (shape.primitive.material.textureMap.isUsed) {
            auto image = loadImageFromFile(shape.primitive.material.textureMap.filename);
            GLuint texture;
            glGenTextures(1, &texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         image.width(), image.height(), 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            scene_textures.emplace(shape.primitive.material.textureMap.filename, texture);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

QImage Realtime::loadImageFromFile(const std::string &file_path) {
    auto file = QString::fromStdString(file_path);
    auto m_image = QImage(file).convertToFormat(QImage::Format_RGBA8888).mirrored();
    return m_image;
}

// Shadow Mapping - Project 6 Extra Credit

void Realtime::makeDepthMaps() {
    // TODO: delete previous texture / fbos?
    directional_light_count = 0;
    for (auto& light : metaData.lights) {
        if (metaData.lights.empty()) {
            break;
        }
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            directional_light_count += 1;
        }
    }
    for (int i = 0; i < directional_light_count; ++i) {
        glGenFramebuffers(1, &m_depth_fbos[i]);
        glGenTextures(1, &m_depth_maps[i]);
        glActiveTexture(GL_TEXTURE0); // do we need this? THIS IS THE LINE WHERE STRANGE BUG OCCURS (MAKES UI DISAPPEAR when set to GL_TEXTURE1)
        glBindTexture(GL_TEXTURE_2D, m_depth_maps[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_fbos[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_maps[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindTexture(GL_TEXTURE_2D, 0);
//        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
        QOpenGLFramebufferObject::bindDefault();
    }
}

void Realtime::paintDepthMaps() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int current_dir_light = 0;
    glUseProgram(m_shadowmapping_shader);
    for (auto& light : metaData.lights) {
        if (metaData.shapes.empty()) {
            break;
        }
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            auto light_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f,
                                               settings.nearPlane, settings.farPlane);
            auto light_inv_dir = glm::vec3(-light.dir);
            auto light_view = glm::lookAt(light_inv_dir,
                                               glm::vec3(0.f, 0.f, 0.f),
                                               glm::vec3(0.f, 1.f, 0.f));
            auto light_space_matrix = light_projection * light_view;
            glUniformMatrix4fv(glGetUniformLocation(m_shadowmapping_shader, "light_space_matrix"),
                               1, GL_FALSE, &light_space_matrix[0][0]);
            // draw to depth map
            glBindFramebuffer(GL_FRAMEBUFFER, m_depth_fbos[current_dir_light]);
            glViewport(0, 0, shadow_width, shadow_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
                auto m_model = shape.ctm;
                glUniformMatrix4fv(glGetUniformLocation(m_shadowmapping_shader, "model_mat_1"),
                                   1, GL_FALSE, &m_model[0][0]);
//                glActiveTexture(GL_TEXTURE0);
//                glBindTexture(GL_TEXTURE_2D, m_depth_maps[current_dir_light]);
                glDrawArrays(GL_TRIANGLES, 0, current_shape->getVertexData().size()/9);
//                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
            }
//            glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
            QOpenGLFramebufferObject::bindDefault();
            current_dir_light += 1;
        }
    }
    glUseProgram(0);
}