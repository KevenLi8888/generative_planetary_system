# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).

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