# Project
This project is a C++ program created using OpenGL, GLFW, GLM and Glad.

The project contains:
* **Models**: cake, coffe, leaves
* **Textures**: grass_skybox
* **Lighting**: directional, spot, and point light implemented using Blinn-Phong lighting model
* **Shaders**: lightshow and skybox shaders
* **Skybox cubemap**: (large)cube that encompases the entire scene, contains 6 images
* **Instancing**: we draw many (equal mesh data) objects at once with a single render call
* **Face culling**: if a side on an object cannot be seen, don't render it
* **Normal mapping**
* **Parallax mapping**
* **By pressing**:
  * **B**: activate/deactivate Blinn-Phong lighting model
  * **V**: switch spot light on/off
