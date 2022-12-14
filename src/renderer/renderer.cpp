#include "renderer/renderer.h"
#include "utils/shaderloader.h"
#include "shape/sphere.h"
#include "shape/cube.h"
#include "shape/cone.h"
#include "shape/cylinder.h"
#include "shape/ring.h"
#include "settings.h"

#include <iostream>
#include <numeric>

// VAO configs
std::vector<int> VAO_POS_NORM_UV_CONFIG { 3, 3, 2 };
std::vector<int> VAO_POS_UV_CONFIG { 3, 2 };
std::vector<int> VAO_POS_CONFIG { 3 };

// Supported implicit shapes
std::vector<PrimitiveType> IMPLICIT_SHAPES {
    PrimitiveType::PRIMITIVE_SPHERE,
    PrimitiveType::PRIMITIVE_CUBE,
    PrimitiveType::PRIMITIVE_CONE,
    PrimitiveType::PRIMITIVE_CYLINDER,
    PrimitiveType::PRIMITIVE_RING,
};

// Fullscreem Quad
std::vector<GLfloat> FULLSCREEN_QUAD_DATA =
{ //     POSITIONS    //
  //       UVS        //
    -1.0f,  1.0f, 0.0f,
     0.0f,  1.0f,
    -1.0f, -1.0f, 0.0f,
     0.0f,  0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f, 0.0f,
     0.0f,  1.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  0.0f,
};

// Final Project
SceneGlobalData DEFAULT_GLOBAL = {
    0.5, 0.5, 0.5,  // ka, kd, ks
};

SceneCameraData DEFAULT_CAMERA = {
    glm::vec4(30, 15, 30, 1),      // pos
    glm::vec4(-3, -1.5, -3, 0), // look
    glm::vec4(0, 1, 0, 0),      // up
    glm::radians(30.0),         // heightAngle
};

std::vector<SceneLightData> DEFAULT_LIGHTS {
    SceneLightData {
        0,
        LightType::LIGHT_POINT,
        glm::vec4(1, 1, 1, 1),
        glm::vec3(1, 0, 0),
        glm::vec4(0, 0, 0, 1),
        glm::vec4(0)
    }
};

std::vector<std::string> DEFAULT_TEXTURES = {
    "resources/images/sun.jpeg",
    "resources/images/mercury.jpeg",
    "resources/images/venus.jpeg",
    "resources/images/earth.jpeg",
    "resources/images/mars.jpeg",
    "resources/images/jupiter.jpeg",
    "resources/images/saturn.jpeg",
    "resources/images/uranus.jpeg",
    "resources/images/neptune.jpeg",
    "resources/images/moon.jpeg",
};

void Renderer::initialize(int screen_w, int screen_h) {
    m_screen_width = screen_w;
    m_screen_height = screen_h;

    // Initialize the fullscreen quad mesh to project on and the FBO
    m_fullscreen_mesh = bindMesh(FULLSCREEN_QUAD_DATA, VAO_POS_UV_CONFIG);
    generateFBO();

    // Final Project
    m_line_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/line.vert",
                "resources/shaders/line.frag"
    );

    m_planet_shader = ShaderLoader::createShaderProgram(
                "resources/shaders/phong.vert",
                "resources/shaders/planet.frag"
    );
}

Renderer::~Renderer() {
    // Clean up all allocated gl resources
    glDeleteBuffers(1, &m_fullscreen_mesh.vbo);
    glDeleteVertexArrays(1, &m_fullscreen_mesh.vao);
    clearGeometryData();
    clearTextureData();
    clearFBO();
}

void Renderer::generateFBO() {
    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_data.texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_data.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_data.renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_data.renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screen_width, m_screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo_data.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_data.fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_data.texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_data.renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_default_fbo);
}

void Renderer::clearFBO() {
    // Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_data.texture);
    glDeleteRenderbuffers(1, &m_fbo_data.renderbuffer);
    glDeleteFramebuffers(1, &m_fbo_data.fbo);
}

// Construct scene from the input .xml file
void Renderer::updateScene(int width, int height) {

    m_ps = PlanetarySystem();

    m_data = {
        DEFAULT_GLOBAL,
        DEFAULT_CAMERA,
        DEFAULT_LIGHTS,
        settings.procedural ? m_ps.generateProceduralSystem() : m_ps.generateSolarSystem()
    };

    m_camera_at = 0;
    m_camera = Camera(width, height, m_data.cameraData);
    m_camera.resetCameraOrbit();

    clearGeometryData();
    clearTextureData();
    updateGeometry();
    generateTextures();

    m_ready = true;
}

// Final Project
void Renderer::updatePlanets(float deltaTime) {
    m_ps.update(deltaTime);
}

// Recompute the mesh data for each type of implicit objects
void Renderer::updateGeometry() {
    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    // Create one mesh for each type of objects
    for (const auto &t: IMPLICIT_SHAPES) {
        m_meshMap[t] = createMesh(t, param1, param2);
    }

}

// Creates a mapping between texture filename and GL Texture
void Renderer::generateTextures() {
    if (!settings.procedural) {
        for (int i = 0; i < planet_type_count; ++i) {
            auto color = m_terrain.generateTerrainColors(i);
            auto resolution = m_terrain.getResolution();
            GLuint color_map;
            glGenTextures(1, &color_map);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, color_map);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         resolution * 2, resolution, 0,
                         GL_RGBA, GL_FLOAT, color.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            m_procedural_texture_map[i] = color_map;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    } else {
        int div = settings.numPlanet / 2 + 1;

        for (int i = 0; i < settings.numPlanet; ++i) {
            std::vector<float> color;
            if (i == 0) {
                color = m_terrain.generateTerrainColors(PlanetType::PLANET_SUN);
            } else if (i <= div) {
                color = m_terrain.generateTerrainColors(PlanetType::PLANET_ROCKY);
            } else {
                color = m_terrain.generateTerrainColors(PlanetType::PLANET_GAS);
            }
            auto resolution = m_terrain.getResolution();
            GLuint color_map;
            glGenTextures(1, &color_map);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, color_map);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         resolution * 2, resolution, 0,
                         GL_RGBA, GL_FLOAT, color.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            m_procedural_texture_map[i] = color_map;
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        for (int i = settings.numPlanet; i < settings.numPlanet + m_ps.getNumMoon(); ++i) {
            auto color = m_terrain.generateTerrainColors(PlanetType::PLANET_MOON);
            auto resolution = m_terrain.getResolution();
            GLuint color_map;
            glGenTextures(1, &color_map);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, color_map);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         resolution * 2, resolution, 0,
                         GL_RGBA, GL_FLOAT, color.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            m_procedural_texture_map[i] = color_map;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    for (int i = 0; i < DEFAULT_TEXTURES.size(); ++i) {
        auto fpath = DEFAULT_TEXTURES[i];
        auto img = QImage(fpath.data()).convertToFormat(QImage::Format_RGBA8888).mirrored();
        GLuint texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_default_texture_map[i] = texture;
    }
}

// Creates the mesh of object type t with the given parameter values
MeshData Renderer::createMesh(PrimitiveType t, int param1, int param2) {
    std::vector<float> mesh;
    switch (t) {
        case PrimitiveType::PRIMITIVE_SPHERE:
            mesh = Sphere::generateShape(param1, param2);
            return bindMesh(mesh, VAO_POS_NORM_UV_CONFIG);
        case PrimitiveType::PRIMITIVE_CUBE:
            mesh = Cube::generateShape(param1, param2);
            return bindMesh(mesh, VAO_POS_NORM_UV_CONFIG);
        case PrimitiveType::PRIMITIVE_CONE:
            mesh = Cone::generateShape(param1, param2);
            return bindMesh(mesh, VAO_POS_NORM_UV_CONFIG);
        case PrimitiveType::PRIMITIVE_CYLINDER:
            mesh = Cylinder::generateShape(param1, param2);
            return bindMesh(mesh, VAO_POS_NORM_UV_CONFIG);
        case PrimitiveType::PRIMITIVE_RING:
            mesh = Ring::generateShape(param1, param2);
            return bindMesh(mesh, VAO_POS_CONFIG);
        default:
            throw std::runtime_error("Shape not supported.");
    }
}

// Allocate and bind VAO and VBO given the mesh data
MeshData Renderer::bindMesh(std::vector<float> &mesh, std::vector<int> &config) {
    GLuint vao, vbo;

    // Generate and bind VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, mesh.size()*sizeof(GLfloat), mesh.data(), GL_STATIC_DRAW);

    // Generate, and bind vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Enable and define attributes based on the configuration
    int start = 0;
    auto stride = std::reduce(config.begin(), config.end());

    for (size_t i = 0; i < config.size(); ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,
                              config[i],
                              GL_FLOAT,
                              GL_FALSE,
                              stride*sizeof(GLfloat),
                              reinterpret_cast<void*>(start*sizeof(GLfloat)));
        start += config[i];
    }

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    return MeshData { vao, vbo, (GLsizei)(mesh.size()/stride) };
}

// Proxy the camera resize/movement calls to the camera class
void Renderer::updateCamera(int width, int height) {
    m_camera.resize(width, height);
}

void Renderer::moveCamera(std::unordered_map<Qt::Key, bool> &key_map, float dist) {
    m_camera.moveCamera(key_map, dist);
}

void Renderer::rotateCamera(float dx, float dy) {
    m_camera.rotateCamera(dx, dy);
}

// Regenerate the FBOs with new dimensions upon resize
void Renderer::resizeCanvas(int screen_w, int screen_h) {
    m_screen_width = screen_w;
    m_screen_height = screen_h;

    clearFBO();
    generateFBO();
}

void Renderer::renderGeometry(GLuint shader) {
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the shader program by calling glUseProgram
    glUseProgram(shader);

    // Update the camera location based on the planet it is looking at
    if (settings.orbitCamera) {
        m_camera.updateCameraView(m_data.shapes[m_camera_at]);
    }

    // Pass in camera related information as uniforms to the shader program
    auto view = m_camera.getViewMatrix();
    auto proj = m_camera.getProjectionMatrix();
    auto proj_view = proj * view;
    auto camera_pos = m_camera.getPosition();

    glUniformMatrix4fv(glGetUniformLocation(shader, "proj_view"), 1, GL_FALSE, &proj_view[0][0]);
    glUniform3fv(glGetUniformLocation(shader, "camera_pos"), 1, &camera_pos[0]);

    // Pass in global color coefficients as uniforms to the shader program
    glUniform1f(glGetUniformLocation(shader, "ka"), m_data.globalData.ka);
    glUniform1f(glGetUniformLocation(shader, "kd"), m_data.globalData.kd);
    glUniform1f(glGetUniformLocation(shader, "ks"), m_data.globalData.ks);

    // Pass in lighting information to the shader program
    int num_lights = std::min((int)m_data.lights.size(), 8);
    glUniform1i(glGetUniformLocation(shader, "num_lights"), num_lights);

    for (int i = 0; i < num_lights; ++i) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        glUniform1i(glGetUniformLocation(shader, (prefix + "type").data()), (int)m_data.lights[i].type);
        glUniform3fv(glGetUniformLocation(shader, (prefix + "color").data()), 1, &m_data.lights[i].color[0]);
        glUniform3fv(glGetUniformLocation(shader, (prefix + "function").data()), 1, &m_data.lights[i].function[0]);
        glUniform3fv(glGetUniformLocation(shader, (prefix + "pos").data()), 1, &m_data.lights[i].pos[0]);
        glUniform3fv(glGetUniformLocation(shader, (prefix + "dir").data()), 1, &m_data.lights[i].dir[0]);
        glUniform1f(glGetUniformLocation(shader, (prefix + "penumbra").data()), m_data.lights[i].penumbra);
        glUniform1f(glGetUniformLocation(shader, (prefix + "angle").data()), m_data.lights[i].angle);
    }

    for (auto &shape: m_data.shapes) {
        auto model = shape->ctm;
        auto primitive = shape->primitive;
        auto model3invt = glm::inverse(glm::mat3(model));
        MeshData mesh = m_meshMap[primitive.type];

        // Bind shape mesh
        glBindVertexArray(mesh.vao);

        // Pass in the model matrix as a uniform to the shader program
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);

        // Also pass in the inverse-transposed 3x3 model matrix to speed up computation
        glUniformMatrix3fv(glGetUniformLocation(shader, "model3invt"), 1, GL_TRUE, &model3invt[0][0]);

        // Pass in material related uniforms
        glUniform3fv(glGetUniformLocation(shader, "material.cAmbient"), 1, &primitive.material.cAmbient[0]);
        glUniform3fv(glGetUniformLocation(shader, "material.cDiffuse"), 1, &primitive.material.cDiffuse[0]);
        glUniform3fv(glGetUniformLocation(shader, "material.cSpecular"), 1, &primitive.material.cSpecular[0]);
        glUniform1f(glGetUniformLocation(shader, "material.shininess"), primitive.material.shininess);

        // Pass in texture related uniforms
        glUniform1i(glGetUniformLocation(shader, "material.usesTexture"), primitive.material.textureMap.isUsed);
        glUniform1f(glGetUniformLocation(shader, "material.repeatU"), primitive.material.textureMap.repeatU);
        glUniform1f(glGetUniformLocation(shader, "material.repeatV"), primitive.material.textureMap.repeatV);
        glUniform1f(glGetUniformLocation(shader, "material.blend"), primitive.material.blend);

        // Load texture if necessasry
        if (shape->primitive.material.textureMap.isUsed) {
            glActiveTexture(GL_TEXTURE0);
            if (!settings.procedural && !settings.proceduralTexture) {
                glBindTexture(GL_TEXTURE_2D, m_default_texture_map[shape->type]);
            } else {
                glBindTexture(GL_TEXTURE_2D, m_procedural_texture_map[shape->type]);
            }
        }

        // Draw shape
        glDrawArrays(GL_TRIANGLES, 0, mesh.size);

        // Unbind Everything
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    if (settings.showOrbits) {
        glUseProgram(m_line_shader);

        for (auto &orbit_ctm: m_ps.getOrbitCtms()) {
            MeshData mesh = m_meshMap[PrimitiveType::PRIMITIVE_RING];

            // Pass in the mvp matrix
            auto mvp = proj_view * orbit_ctm;
            glUniformMatrix4fv(glGetUniformLocation(m_line_shader, "mvp"), 1, GL_FALSE, &mvp[0][0]);

            // Bind shape mesh
            glBindVertexArray(mesh.vao);

            // Draw orbit ring
            glDrawArrays(GL_LINE_STRIP, 0, mesh.size);

            // Unbind Everything
            glBindVertexArray(0);
        }
    }

    // Deactivate the shader program
    glUseProgram(0);
}

void Renderer::renderFBO(GLuint shader) {
    glUseProgram(shader);

    // Bind fullscreen quad VAO and FBO texture
    glBindVertexArray(m_fullscreen_mesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_data.texture);

    // Draw on fullscreen quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind all
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::render(GLuint phong_shader, GLuint texture_shader) {
    // Render geometries to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_data.fbo);
    renderGeometry(phong_shader);

    // Render the buffer image to screen
    glBindFramebuffer(GL_FRAMEBUFFER, m_default_fbo);
    renderFBO(texture_shader);
}

void Renderer::clearGeometryData() {
    // Recycle all geometry VAOs and VBOs
    for (auto &it: m_meshMap) {
        glDeleteBuffers(1, &it.second.vbo);
        glDeleteVertexArrays(1, &it.second.vao);
    }
    m_meshMap.clear();
}

void Renderer::clearTextureData() {
    // Recycle all textures
    for (auto &it: m_default_texture_map) {
        glDeleteTextures(1, &it.second);
    }
    for (auto &it: m_procedural_texture_map) {
        glDeleteTextures(1, &it.second);
    }

    m_default_texture_map.clear();
    m_procedural_texture_map.clear();
}

void Renderer::clearSceneData() {
    for (auto &it: m_data.shapes) {
        delete it;
    }
}

// Final Project
void Renderer::switchCamera(std::unordered_map<Qt::Key, bool> &key_map, float time) {
    m_last_switch += time;
    if (m_last_switch > 0.3) {
        auto prev_camera_at = m_camera_at;
        if (key_map[Qt::Key_A]) m_camera_at -= 1;
        if (key_map[Qt::Key_D]) m_camera_at += 1;

        if (m_camera_at < 0) m_camera_at = m_data.shapes.size()-1;
        else if (m_camera_at >= m_data.shapes.size()) m_camera_at = 0;

        if (m_camera_at != prev_camera_at) {
            m_camera.resetCameraOrbit();
            m_last_switch = 0;
        }
    }
}

void Renderer::replaceCamera(int width, int height) {
    m_camera = Camera(width, height, m_data.cameraData);
    if (settings.orbitCamera) {
        m_camera.resetCameraOrbit();
        m_camera_at = 0;
    }
}
