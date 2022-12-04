# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).

# Project 6: Action!
DEFAULT FBO: The default framebuffer is bind using a different approach (`QOpenGLFramebufferObject::bindDefault()`) because an indefinite number of FBOs are generated for shadow mapping. There should be no need to change `m_defaultFBO` . It’s value can be changed in `realtime.h line 113`. All calls using `m_defaultFBO` are commented out in `realtime.cpp`, and you can find them by searching.

Most of the new features in this project are located in the Realtime class (`realtime.h/.cpp`).

## Lights

`default.frag`

Added lighting computation for point lights and spot lights in the default fragment shader.

## Camera Movement

### Translational Movement (Keyboard)

`Realtime::timerEvent() (realtime.cpp line 564)`

Updates the camera position according to keyboard inputs.

### Rotational Movement (Mouse)

`Realtime::mouseMoveEvent() (realtime.cpp line 544)`

Updates the camera look vector by multiplying a rotation matrix and the original look vector.

The rotation matrix is constructed in `Camera::getRotationMatrix() (camera.cpp line 108)`.

The rotation direction matches the direction of the mouse movement, providing a natural “drag” feeling.

## Filters

Post-processing filters are applied in the final stage of a two-pass rendering like the one in lab 11. The rendering codes are in `paintGL()`. And the shaders used in the second pass are `postprocessing.vert` and `postprocessing.frag`.

`Realtime::initializeFullscreenQuad() (realtime.cpp line 609)`

`Realtime::makeFBO() (realtime.cpp line 659)`

These two functions are called in `initializeGL()` to set up VBO, VAO and FBOs needed for the second pass rendering.

`makeFBO()` is also called in `resizeGL()` to make the output matches the screen size correctly.

`Realtime::paintPostProcess() (realtime.cpp line 689)`

This function passes the currently activated filter to the postprocessing shader, and draw to the default FBO.

## Extra Credits

The project includes three rendering passes and shaders.

Depth Mapping (`shadowmapping` shader) → Lighting, Texture, Shadow Calculation (`default` shader) → Post Processing (`postprocessing` shader)

### 1 - Implement More Filters

`postprocessing.frag`

Implemented one more per-pixel and kernel-based filter respectively. Filters can be toggled via checkboxes. The effects of per-pixel filters are stackable, while kernel-based filters are not stackable.

### 2 - Directional Light Shadow Maps

Used depth maps as textures for shadow mapping which adds another render pass. Up to 8 directional lights are supported.

**Creating depth maps**

`Realtime::makeDepthMaps() (realtime.cpp line 741)`

Generate depth textures that will be rendered from the lights’ perspective and then used to test for shadows.

Since depth maps are lights dependent (scene-dependent), this function is called in `sceneChanged()`.

**Render to depth maps**

`Realtime::printDepthMaps() (realtime.cpp line 773)`

`shadowmapping.vert/.frag`

Make a light-space transform to the world space position, and render the scene from the light’s point of view.

**Testing and rendering shadows**

`Realtime::paintScene() (realtime.cpp line 260)`

This step is performed within the existing `paintScene()` process. The `default` shader is modified to process light-space transforms in the vertex shader, and current depth is calculated and compared to depth maps in the fragment shader (passed in as `sampler2D`s) to determine whether the fragment is in shadow. Then the shadow info is used in lighting calculation.

![Shadow mapping example](/images/shadowmapping.png)

Example of Shadow Mapping using scenefile `directional_light_2.xml`

### 3 - Texture Mapping

`Primitive::getUV() (primitive.h/.cpp)`

UV coordinates are added to shapes and used for texture mapping. Offsets are added in edge cases for OpenGL to correctly handle the coordinate interpolation.

`Realtime::loadTextures() (realtime.cpp line 710)`

Initialize textures only once for each scene. When a new scene is loaded, read textures from files, generate and bind images to texture.

`Realtime::paintScene() (realtime.cpp line 260)`

Textures are passed into the default fragment shader as `sampler2D` uniforms.

![Texture mapping example](/images/texturemapping.png)

Example of Texture Mapping using scenefile `texture_cheese.xml`

# Project 5: Lights, Camera
## Project Pipeline

### Shape Tessellation

`class Primitive (primitive.h/.cpp)`

Four child classes - Sphere, Cylinder, Cone and Cube, derived from the parent class Primitive.

### Camera

`class Camera (camera.h/.cpp)`

The projection matrix is implemented in `Camera::getProjectionMatrix()`.

The camera is instantiated when `Realtime::sceneChanged()` is called, and updated when the screen is resized, or settings are updated (near and far plane).

### Data Handling

`class Primitive (primitive.h/.cpp)`

Each type of shape (each primitive instance) has its own VAO and VBO (i.e. 4 VAOs and 4 VBOs in total). Their ids are stored in member variables and use member functions `makeVBO()`, `makeVAO()` to initialize, bind and fill data.

`Realtime::initializeShapes() (realtime.cpp line 207)`

initializeShapes() calls `makeVBO()`, `makeVAO()` to construct/reconstruct and bind VBO/VAOs when the scene is first loaded or settings are changed.

### Shaders

`default.vert`

Passed in the inversed and transposed model matrix from CPU to avoid calculating inverses in the shader.

`default.frag`

Used structs to store global data, shape data and light data in the fragment shader.

### Scene Rendering

`Realtime::paintGL() (realtime.cpp line 81)`

For each shape in the scene, obtain and bind the VAO based on its type, and pass data into shaders. Call `glDrawArrays()` and unbind the VAO.

## Extra Credit (Adaptive Level of Detail)

The level of detail is divided into three levels:

- High (current tessellation parameters * 3)
- Medium (equals to current shape parameter)
- Low (current tessellation parameters * 0.5)

Another 8 Primitive class instances are used to store corresponding VBO/VAOs. (2 new levels and 4 types of shapes, with the suffix `_hi` or `_low`. The medium level uses the default/existing instance without a suffix.)

The level of detail is determined in the `paintGL()` render loop. Then the corresponding VAO is bound. (`realtime.cpp line 89-100`).

### Based on the number of objects in the scene

**Toggle with extra credit checkbox 1.**

`Realtime::getDetailLevelByNumberOfObjects() (realtime.cpp line 291)`

- Object number < 50: High level of detail.
- 50 ≤ Object number < 200: Medium level of detail.
- Object number ≥ 200: Low level of detail.

### Based on the distance from the object to the camera

**Toggle with extra credit checkbox 2.**

`Realtime::getDetailLevelByDistance() (realtime.cpp line 274)`

The object distance to the camera is calculated in the canonical perspective view volume (transformed with $s\_xyz * camera\_view\_matrix * ctm$). With a far plane of z = -1, this ensures a maximum distance of 1 between the object and camera.

The distance is not calculated in camera space because the maximum distance is not determined and requires more operations to figure out the maximum distance then divide the objects to different levels according to that.

The distance is also not calculated in the canonical space because the unhinging operation might makes the distance not linearly increases across the view volume.

- Distance < 0.2: High level of detail.
- 0.2 ≤ Distance < 0.5: Medium level of detail.
- Distance ≥ 0.5: Low level of detail.

### Notes

1. The two ways of varying level of detail cannot be enabled at the same time. If both checkboxes are toggled, the varying by distance approach will be applied.
2. Using low tessellation parameters can make the adaptive effect more noticeable.
3. Below are examples using `recursiveSpheres3.xml`.

![No adaptive level of detail](/images/default.png)

No adaptive level of detail

![Adaptive based on numbers (level-high)](/images/number.png)

Adaptive based on numbers (level-high)

![Adaptive based on distance](/images/distance.png)

Adaptive based on distance