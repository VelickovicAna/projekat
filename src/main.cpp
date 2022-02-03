#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <cstdlib>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void renderQuad(); 
void renderQuad1();

// settings
float heightScale = 0.1;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool blinn = false;
bool blinnKeyPressed = false;
bool flashLight = false;
bool flashLightKeyPressed = false;

// camera 
Camera camera(glm::vec3(0.0f, 3.0f, 15.0f));
//Camera camera(glm::vec3(0.0f, 1.0f, 12.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cakeposition(3.15f, -0.7f, 1.0f);
glm::vec3 coffeeposition(-3.5f, -0.5f, 3.0f);

void set_spot_light(Shader& shader, Camera& camera);

bool isSpotlightActivated = true;

int main(){
  
      // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "***", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Shader initialization
    // -------------------------
    Shader grassboxShader("resources/shaders/grassbox.vs", "resources/shaders/grassbox.fs");
    Shader lightshowShader("resources/shaders/lightshow.vs", "resources/shaders/lightshow.fs");
    Shader shader("resources/shaders/blending.vs", "resources/shaders/blending.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // tablecloth plain coordinates
    float floorVertices[] = {
        // positions          // normals          // texture coords
         0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  1.0f,  // top right
         0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  0.0f,  // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,  // bottom left
        -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  1.0f   // top left
    };

    // tablecloth vertices for use in EBO
    unsigned int floorIndices[] = {
         0, 1, 3,  // first Triangle
         1, 2, 3   // second Triangle
    };

    // grassbox coordinates
    float grassboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
  
    // tablecloth setup
    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // grassbox setup
    unsigned int grassboxVAO, grassboxVBO;
    glGenVertexArrays(1, &grassboxVAO);
    glGenBuffers(1, &grassboxVBO);

    glBindVertexArray(grassboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassboxVertices), &grassboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

// transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
  
    // loading textures into shaders
    // -------------

    // lightshow textures
    unsigned int cakeSpecularRed = loadTexture(FileSystem::getPath("resources/objects/slice_of_cake/red.jpeg").c_str());
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/grass.png").c_str());

    // transparent vegetation locations
    // --------------------------------
    vector<glm::vec3> vegetation
            {
                    glm::vec3(-11.0f, 0.0f, 13.0f),
                    glm::vec3(-11.6f, 0.0f, 13.0f),
                    glm::vec3(-10.0f, 0.0f, 13.0f),
                    glm::vec3(-10.5f, 0.0f, 13.0f),
                    glm::vec3(-8.0f, 0.0f, 13.0f),
                    glm::vec3(-7.0f, 0.0f, 13.0f),
                    glm::vec3( -6.0f, 0.0f, 13.0f),
                    glm::vec3( -5.0f, 0.0f, 13.0f),
                    glm::vec3(-4.0f, 0.0f, 13.0f),
                    glm::vec3 (-3.0f, 0.0f, 13.0f),
                    glm::vec3(-2.0f, 0.0f, 13.0f),
                    glm::vec3(-1.0f, 0.0f, 13.0f),
                    glm::vec3(0.0f, 0.0f, 13.0f),
                    glm::vec3(1.0f, 0.0f, 13.0f),
                    glm::vec3(2.3f, 0.0f, 13.0f),
                    glm::vec3(3.3f, 0.0f, 13.0f),
                    glm::vec3(4.3f, 0.0f, 13.0f),
                    glm::vec3(4.8f, 0.0f, 13.0f),
                    glm::vec3(7.0f, 0.0f, 13.0f),
                    glm::vec3(8.0f, 0.0f, 13.0f),
                    glm::vec3(10.0f, 0.0f, 13.0f)
            };

    lightshowShader.use();
    lightshowShader.setInt("material.diffuse", 0);
    lightshowShader.setInt("material.specular", 1);
    lightshowShader.setInt("material.specular2", 2);

    // tablecloth textures
    unsigned int floorDiffuseMap = loadTexture(FileSystem::getPath("resources/textures/images.jpeg").c_str());

    // grassbox textures
    vector<std::string> grassboxSides = {
        FileSystem::getPath("resources/textures/trava/sh_rt.png"),
        FileSystem::getPath("resources/textures/trava/sh_lf.png"),
        FileSystem::getPath("resources/textures/trava/sh_up.png"),
        FileSystem::getPath("resources/textures/trava/sh_dn.png"),
        FileSystem::getPath("resources/textures/trava/sh_bk.png"),
        FileSystem::getPath("resources/textures/trava/sh_ft.png")
    };
    unsigned int cubemapTexture = loadCubemap(grassboxSides);
    grassboxShader.use();
    grassboxShader.setInt("grassbox", 0);


    shader.use();
    shader.setInt("texture1", 0);

    // load model
    Model cake(FileSystem::getPath("resources/objects/slice_of_cake/cake.obj"));
    Model coffee(FileSystem::getPath("resources/objects/coffee/coffee_cup_obj.obj"));
    cake.SetShaderTextureNamePrefix("material.");
    coffee.SetShaderTextureNamePrefix("material.");
    Model leaf(FileSystem::getPath("resources/objects/leaf/leaf.obj"));
    leaf.SetShaderTextureNamePrefix("material.");




    glm::vec3 lightPos((5.15f, -0.5f, 4.0f));
  
    //unsigned int normalMap  = loadTexture(FileSystem::getPath("resources/objects/slice_of_cake/cake_texture_by_xnickixstockx.jpg").c_str());
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/objects/slice_of_cake/cake_texture_by_xnickixstockx.jpg").c_str());
    unsigned int normalMap  = loadTexture(FileSystem::getPath("resources/objects/slice_of_cake/cake_texture_by_xnickixstockxbump1.jpg").c_str());
  
    unsigned int diffuseMap1 = loadTexture(FileSystem::getPath("resources/objects/coffee/metallic.png").c_str());
    unsigned int normalMap1  = loadTexture(FileSystem::getPath("resources/objects/coffee/normal.png").c_str());
    unsigned int heightMap1 = loadTexture(FileSystem::getPath("resources/objects/coffee/roughness.png").c_str());
  
  
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
      // per-frame time logic  
        // --------------------
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float time = currentFrame;
      
      
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
        // lightshowShader setup 
        // -----------
        lightshowShader.use();
        lightshowShader.setVec3("viewPos", camera.Position);
        lightshowShader.setFloat("material.shininess", 16.0f);
        lightshowShader.setInt("blinn", blinn);
        lightshowShader.setInt("flashLight", flashLight);
      
        // directional light setup
        lightshowShader.setVec3("dirLight.direction", 1.0f, -0.5f, 0.0f);
        lightshowShader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        lightshowShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightshowShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
      
        // point light setup 
        //lightshowShader.setVec3("pointLight.position", cakeposition);
        lightshowShader.setVec3("pointLight.position", glm::vec3(5.15f, 9.0f, 4.0f));
        lightshowShader.setVec3("pointLight.ambient", 0.01f, 0.01f, 0.01f);
        lightshowShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightshowShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        lightshowShader.setFloat("pointLight.constant", 1.0f);
        lightshowShader.setFloat("pointLight.linear", 0.05f);
        lightshowShader.setFloat("pointLight.quadratic", 0.012f);
      
        //set_spot_light(lightshowShader);
        // spotlight setup(5.15f, -0.5f, 4.0f)
        lightshowShader.setVec3("spotLight.position", glm::vec3(5.15f, 9.0f, 4.0f));
        lightshowShader.setVec3("spotLight.direction", lightPos);
        lightshowShader.setVec3("spotLight.ambient", 1.0f, 1.0f, 1.0f);
        lightshowShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightshowShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightshowShader.setFloat("spotLight.constant", 0.3f);
        lightshowShader.setFloat("spotLight.linear", 0.1);
        lightshowShader.setFloat("spotLight.quadratic", 0.1);
        lightshowShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(17.9f)));
        lightshowShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));
        //lightshowShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));
      
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightshowShader.setMat4("projection", projection);
        lightshowShader.setMat4("view", view);

        glm::mat4 model(1.0f);
      
        // world transformations
        model = glm::mat4(1.0f);

        model = glm::translate(model, cakeposition);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        lightshowShader.setMat4("model", model);
      
        // render cake 
        cake.Draw(lightshowShader);
        glm::mat4 model1(1.0f);
        model1 = glm::translate(model1, coffeeposition);
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
        model1 = glm::scale(model1, glm::vec3(4.5f));
        lightshowShader.setMat4("model", model1);

        coffee.Draw(lightshowShader);
      
        // tablecloth setup
        // light properties
        lightshowShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // material properties
        lightshowShader.setFloat("material.shininess", 256.0f);

        // world transformation
       model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(25.0f));
        lightshowShader.setMat4("model", model);
      
        // bind diffuse map 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);

        // render tablecloth
        glBindVertexArray(floorVAO);
        glEnable(GL_CULL_FACE);     // floor won't be visible if looked from bellow
        glCullFace(GL_BACK);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_CULL_FACE);

      
        // grassbox shader setup 
        // -----------
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        grassboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        grassboxShader.setMat4("view", view);
        grassboxShader.setMat4("projection", projection);

        // render grassbox cube 
       glBindVertexArray(grassboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        

        // vegetation
        shader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = camera.GetViewMatrix();
        model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (unsigned int i = 0; i < vegetation.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, vegetation[i]);
            model = glm::scale(model, glm::vec3(3.0f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
      // optional: de-allocate all resources once they've outlived their purpose:
    // -----------------------------------------------------------------------
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &grassboxVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &floorEBO);
    glDeleteBuffers(1, &grassboxVBO);
    // destroy all remaining windows/cursors, free any allocated resources
    glfwTerminate();
    return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // switch Blinn-Phong lighting model on/off
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
        if (blinn)
            cout << "Blinn-Phong" << endl;
        else
            cout << "Phong" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    // switch Spot light on/off
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !flashLightKeyPressed)
    {
        flashLight = !flashLight;
        flashLightKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
    {
        flashLightKeyPressed = false;
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO1 = 0;
unsigned int quadVBO1;
void renderQuad1()
{
    if (quadVAO1 == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO1);
        glGenBuffers(1, &quadVBO1);
        glBindVertexArray(quadVAO1);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void set_spot_light(Shader& objectShader, Camera& camera) {
    objectShader.setVec3("spotLight.position", glm::vec3(1.0f, 1.0f, 12.0f));
    objectShader.setVec3("spotLight.direction", -camera.Up);
    if(isSpotlightActivated){
        objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    }
    else{ // All to 0.
        objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        objectShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        objectShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
    }
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.01);
    objectShader.setFloat("spotLight.quadratic", 0.001);
    objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(0.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(22.0f)));

    objectShader.setVec3("viewPos", camera.Position);
    objectShader.setFloat("material.shininess", 128.0f);
}
