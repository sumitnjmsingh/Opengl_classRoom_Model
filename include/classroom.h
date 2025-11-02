#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shader.h"
#include "model.h"

class Classroom
{
public:
    // Classroom dimensions (in meters, typical for Indian classroom)
    static constexpr float ROOM_WIDTH = 12.0f;
    static constexpr float ROOM_LENGTH = 8.0f;
    static constexpr float ROOM_HEIGHT = 3.5f;
    static constexpr float WALL_THICKNESS = 0.2f;

    // VAOs and VBOs for different components
    unsigned int floorVAO, floorVBO;
    unsigned int ceilingVAO, ceilingVBO;
    unsigned int wallsVAO, wallsVBO;
    unsigned int doorsVAO, doorsVBO;
    unsigned int windowsVAO, windowsVBO;
    unsigned int benchesVAO, benchesVBO;
    unsigned int podiumVAO, podiumVBO;
    unsigned int boardVAO, boardVBO;
    unsigned int lightsVAO, lightsVBO;

    // OBJ models
    Model fanModel;
    Model podiumModel;
    Model benchModel;
    float fanRotation;

    // Vertex data containers
    std::vector<float> floorVertices;
    std::vector<float> ceilingVertices;
    std::vector<float> wallVertices;
    std::vector<float> doorVertices;
    std::vector<float> windowVertices;
    std::vector<float> benchVertices;
    std::vector<float> podiumVertices;
    std::vector<float> boardVertices;
    std::vector<float> lightVertices;

    Classroom();
    ~Classroom();

    void initializeGeometry();
    void render(Shader& shader);
    void renderLights(Shader& lightShader);
    void updateFan(float deltaTime);
    void renderFan(Shader& shader);
    void renderPodium(Shader& shader);
    void renderBenches(Shader& shader);

private:
    void generateFloor();
    void generateCeiling();
    void generateWalls();
    void generateDoors();
    void generateWindows();
    void generateBenches();
    void generatePodium();
    void generateGreenBoard();
    void generateLights();

    void addQuad(std::vector<float>& vertices, 
                glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, 
                glm::vec3 normal, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4);
    
    void addCube(std::vector<float>& vertices, 
                glm::vec3 position, glm::vec3 size);

    void setupBuffers(unsigned int& VAO, unsigned int& VBO, const std::vector<float>& vertices);
    void renderBuffer(unsigned int VAO, size_t vertexCount, const glm::vec3& materialAmbient, 
                     const glm::vec3& materialDiffuse, const glm::vec3& materialSpecular, 
                     Shader& shader);
};

#endif