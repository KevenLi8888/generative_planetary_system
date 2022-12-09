#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>

#include "camera/camera.h"
#include "planet/planetarysystem.h"
#include <unordered_map>

struct MeshData {
    GLuint vao;
    GLuint vbo;
    GLsizei size;
};

struct FBOData {
    GLuint fbo;
    GLuint texture;
    GLuint renderbuffer;
};

class Renderer
{
public:
    ~Renderer();
    void initialize(int screen_w, int screen_h);
    void updateScene(int width, int height);
    void updateGeometry();
    void updatePlanets(float deltaTime);
    void updateCamera(int width, int hieght);
    void moveCamera(std::unordered_map<Qt::Key, bool> &key_map, float dist);
    void rotateCamera(float dx, float dy);
    void resizeCanvas(int screen_w, int screen_h);
    void render(GLuint phong_shader, GLuint texture_shader);
    void clearGeometryData();
    void clearTextureData();
    void clearSceneData();
    void clearFBO();

private:
    int m_screen_width;
    int m_screen_height;
    RenderData m_data;
    Camera m_camera;

    // Resources for post-processing effects
    GLuint m_default_fbo = 2;
    FBOData m_fbo_data;
    MeshData m_fullscreen_mesh;
    void generateFBO();

    // Paint functions
    void renderGeometry(GLuint shader);
    void renderFBO(GLuint shader);

    // Mesh related gl resources and methods
    std::unordered_map<PrimitiveType, MeshData> m_meshMap;
    MeshData createMesh(PrimitiveType t, int param1, int param2);
    MeshData bindMesh(std::vector<float> &mesh, std::vector<int> &config);

    // Texture related resources
   std::unordered_map<std::string, GLuint> m_texture_map;
   void generateTextures();

   // Final Project
   PlanetarySystem m_ps;
};
