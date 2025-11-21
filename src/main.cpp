#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Include our custom headers
#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/classroom.h"

// Window dimensions
const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 800;

// Camera - positioned at front of classroom looking toward back wall green board
Camera camera(glm::vec3(0.0f, 2.0f, 3.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Global pointer to classroom for input handling
Classroom* g_classroom = nullptr;

// Key press tracking to avoid multiple toggles
bool keyPressed[6] = {false, false, false, false, false, false};
bool screenKeyPressed = false;  // For projector screen toggle (P key)

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);  // Enable 4x multisampling anti-aliasing

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CL-3 Classroom (South Campus)", NULL, NULL);
    if (window == NULL)
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

    // glew: load all OpenGL function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  // Enable multisampling for smooth edges
    glEnable(GL_LINE_SMOOTH);  // Enable line smoothing
    glEnable(GL_POLYGON_SMOOTH);  // Enable polygon smoothing
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Best quality line smoothing
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);  // Best quality polygon smoothing

    // build and compile our shader program
    Shader lightingShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader lightCubeShader("shaders/light_vertex.glsl", "shaders/light_fragment.glsl");

    // Initialize classroom
    Classroom classroom;
    classroom.initializeGeometry();
    
    // Set global pointer for input handling
    g_classroom = &classroom;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // Set up 4 corner ceiling lights
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);
        
        // Light positions at four corners of ceiling (room is 12m x 8m)
        glm::vec3 lightPositions[4] = {
            glm::vec3(-4.2f, 3.0f, -2.2f),         // Front-left corner
            glm::vec3(4.2f, 3.0f, -2.2f),          // Front-right corner
            glm::vec3(-4.2f, 3.0f, 2.2f),          // Back-left corner
            glm::vec3(4.2f, 3.0f, 2.2f)            // Back-right corner
        };
        
        // Set number of active lights
        lightingShader.setInt("numActiveLights", 4);
        
        // Configure each corner light
        for(int i = 0; i < 4; i++) {
            std::string lightBase = "lights[" + std::to_string(i) + "]";
            lightingShader.setVec3(lightBase + ".position", lightPositions[i]);
            lightingShader.setVec3(lightBase + ".ambient", 0.16f * lightColor);
            lightingShader.setVec3(lightBase + ".diffuse", 0.20f * lightColor);
            lightingShader.setVec3(lightBase + ".specular", 0.35f * lightColor);
        }

        // Default material properties (will be overridden in classroom.render)
        lightingShader.setVec3("material.ambient", glm::vec3(0.5f));
        lightingShader.setVec3("material.diffuse", glm::vec3(0.7f));
        lightingShader.setVec3("material.specular", glm::vec3(0.3f));
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setFloat("alpha", 1.0f);  // Default to fully opaque

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the classroom
        classroom.render(lightingShader);
        
        // Update and render the fan
        classroom.updateFan(deltaTime);
        classroom.renderFan(lightingShader);
        
        // Update projector screen animation
        classroom.updateProjectorScreen(deltaTime);

        // render light sources
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // Render ceiling lights
        classroom.renderLights(lightCubeShader);

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input
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
    
    // Fan control keys (1-6)
    if (g_classroom != nullptr) {
        // Key 1 - Fan 0
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            if (!keyPressed[0]) {
                g_classroom->toggleFan(0);
                keyPressed[0] = true;
            }
        } else {
            keyPressed[0] = false;
        }
        
        // Key 2 - Fan 1
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            if (!keyPressed[1]) {
                g_classroom->toggleFan(1);
                keyPressed[1] = true;
            }
        } else {
            keyPressed[1] = false;
        }
        
        // Key 3 - Fan 2
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            if (!keyPressed[2]) {
                g_classroom->toggleFan(2);
                keyPressed[2] = true;
            }
        } else {
            keyPressed[2] = false;
        }
        
        // Key 4 - Fan 3
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            if (!keyPressed[3]) {
                g_classroom->toggleFan(3);
                keyPressed[3] = true;
            }
        } else {
            keyPressed[3] = false;
        }
        
        // Key 5 - Fan 4
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (!keyPressed[4]) {
                g_classroom->toggleFan(4);
                keyPressed[4] = true;
            }
        } else {
            keyPressed[4] = false;
        }
        
        // Key 6 - Fan 5
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (!keyPressed[5]) {
                g_classroom->toggleFan(5);
                keyPressed[5] = true;
            }
        } else {
            keyPressed[5] = false;
        }
        
        // Key P - Toggle Projector Screen
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (!screenKeyPressed) {
                g_classroom->toggleProjectorScreen();
                screenKeyPressed = true;
            }
        } else {
            screenKeyPressed = false;
        }
    }
}

// glfw: whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);

    if (firstMouse)
    {
        lastX = xposf;
        lastY = yposf;
        firstMouse = false;
    }

    float xoffset = xposf - lastX;
    float yoffset = lastY - yposf; // reversed since y-coordinates go from bottom to top

    lastX = xposf;
    lastY = yposf;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}