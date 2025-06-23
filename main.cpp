#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include  "include/Filesystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/Shader.h"
#include "Camera.h"

#include <iostream>
#include "classes/Block/Block.h"
#include <string>
#include <iomanip>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

std::string version = "1.0.1";

int main() {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, ("MeinCraft:" + version).c_str(), nullptr, nullptr);
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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader BlockShader((FileSystem::getPath("Shaders/Block/vertex.glsl").c_str()), FileSystem::getPath("Shaders/Block/frag.glsl").c_str());
    Shader lightCubeShader(FileSystem::getPath("Shaders/Light_source/vertex.glsl").c_str(), FileSystem::getPath("Shaders/Light_source/frag.glsl").c_str() );
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, BlockVao, EBO;
    glGenVertexArrays(1, &BlockVao);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO first
    glBindVertexArray(BlockVao);

    // Bind and set VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Bind and set EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VAO (optional)
    glBindVertexArray(0);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightBlockVao;
    glGenVertexArrays(1, &lightBlockVao);
    glBindVertexArray(lightBlockVao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/pack.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/specular.png").c_str());

    // shader configuration
    // --------------------
    BlockShader.use();
    BlockShader.setInt("material.diffuse", 0);
    BlockShader.setInt("material.specular", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::cout << "\rfps: " << std::fixed << std::setprecision(1) << (1.0 / deltaTime) << std::flush;


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor( 0.3608f, 0.7137f, 1.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        BlockShader.use();
        BlockShader.setVec3("viewPos", camera.Position);
        BlockShader.setFloat("material.shininess", 1.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // sky setup
        float inGameTime = fmod(glfwGetTime() / 10.0f, 1.0f);
        BlockShader.setFloat("Sky.time", inGameTime);
        BlockShader.setVec3("dirLight.ambient", glm::vec3(0.1f, 0.1, 0.1f));
        BlockShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        BlockShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // directional light
        BlockShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        BlockShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        BlockShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        BlockShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // point light 1 For torches
        BlockShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        BlockShader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        BlockShader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        BlockShader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setFloat("pointLights[0].constant", 1.0f);
        BlockShader.setFloat("pointLights[0].linear", 0.09f);
        BlockShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        BlockShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        BlockShader.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        BlockShader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        BlockShader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setFloat("pointLights[1].constant", 1.0f);
        BlockShader.setFloat("pointLights[1].linear", 0.09f);
        BlockShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        BlockShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        BlockShader.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        BlockShader.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        BlockShader.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setFloat("pointLights[2].constant", 1.0f);
        BlockShader.setFloat("pointLights[2].linear", 0.09f);
        BlockShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        BlockShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        BlockShader.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        BlockShader.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        BlockShader.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setFloat("pointLights[3].constant", 1.0f);
        BlockShader.setFloat("pointLights[3].linear", 0.09f);
        BlockShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        BlockShader.setVec3("spotLight.position", camera.Position);
        BlockShader.setVec3("spotLight.direction", camera.Front);
        BlockShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        BlockShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        BlockShader.setFloat("spotLight.constant", 1.0f);
        BlockShader.setFloat("spotLight.linear", 0.09f);
        BlockShader.setFloat("spotLight.quadratic", 0.032f);
        BlockShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        BlockShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        std::vector<float> world_vertices;
        std::vector<unsigned int> world_indices;

        for (int x = -10; x < 10; x++)
            for (int y = 0; y < 10; y++)
                for (int z = -10; z < 10; z++) {
                    Block block;

                    std::vector<float> blockvertices = block.assemble_vertices({0,1,2,3,4,5});

                    // Apply transform to vertices here (optional)
                    for (size_t i = 0; i < blockvertices.size(); i += 8) {
                        blockvertices[i + 0] += x; // x
                        blockvertices[i + 1] += y; // y
                        blockvertices[i + 2] += z; // z
                    }

                    std::vector<unsigned int> blockindices = block.assemble_indices({0,1,2,3,4,5});
                    unsigned int vertexOffset = world_vertices.size() / 8;

                    for (auto& index : blockindices)
                        index += vertexOffset;

                    world_vertices.insert(world_vertices.end(), blockvertices.begin(), blockvertices.end());
                    world_indices.insert(world_indices.end(), blockindices.begin(), blockindices.end());
                }

        // Upload and draw
        glBindVertexArray(BlockVao);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, world_vertices.size() * sizeof(float), world_vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, world_indices.size() * sizeof(unsigned int), world_indices.data(), GL_DYNAMIC_DRAW);

        BlockShader.use();
        BlockShader.setMat4("view", view);
        BlockShader.setMat4("projection", projection);
        BlockShader.setMat4("model", glm::mat4(1.0f)); // model baked into vertex

        glDrawElements(GL_TRIANGLES, world_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);




         // also draw the lamp object(s)
         lightCubeShader.use();
         lightCubeShader.setMat4("projection", projection);
         lightCubeShader.setMat4("view", view);

         // we now draw as many light bulbs as we have point lights.
         glBindVertexArray(lightBlockVao);
         for (unsigned int i = 0; i < 4; i++)
         {

             model = glm::mat4(1.0f);
             model = glm::translate(model, pointLightPositions[i]);
             model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
             lightCubeShader.setMat4("model", model);
             glDrawArrays(GL_TRIANGLES, 0, 36);
         }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &BlockVao);
    glDeleteVertexArrays(1, &lightBlockVao);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}