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
    unsigned int ceilingSlabVAO, ceilingSlabVBO;  // Solid ceiling slab
    unsigned int ceilingTilesVAO, ceilingTilesVBO; // Decorative tiles
    unsigned int wallsVAO, wallsVBO;
    unsigned int doorsVAO, doorsVBO;
    unsigned int windowsVAO, windowsVBO;
    unsigned int benchesVAO, benchesVBO;
    unsigned int podiumVAO, podiumVBO;
    unsigned int boardVAO, boardVBO;
    unsigned int boardBorderVAO, boardBorderVBO;
    unsigned int lightsVAO, lightsVBO;
    unsigned int clockVAO, clockVBO;

    // OBJ models
    Model fanModel;
    Model podiumModel;
    Model benchModel;
    Model projectorModel;
    Model projectorV2Model;  // Hanging projector
    Model clockModel;
    float fanRotation;
    bool fanRunning[6];  // Track which fans are running (0-5 for 6 fans)
    
    // Projector screen state
    float screenExtension;  // 0.0 (rolled up) to 1.0 (fully extended)
    bool screenRolling;     // Whether screen is currently rolling
    bool screenExtending;   // True = extending down, False = rolling up

    // Vertex data containers
    std::vector<float> floorVertices;
    std::vector<float> ceilingVertices;
    std::vector<float> ceilingSlabVertices;  // Solid ceiling slab vertices
    std::vector<float> ceilingTilesVertices; // Decorative tiles vertices
    std::vector<float> wallVertices;
    std::vector<float> doorVertices;
    std::vector<float> windowVertices;
    std::vector<float> benchVertices;
    std::vector<float> podiumVertices;
    std::vector<float> boardVertices;
    std::vector<float> boardBorderVertices;
    std::vector<float> lightVertices;
    std::vector<float> clockVertices;
    
    // Texture IDs
    unsigned int benchTextureID;
    unsigned int ceilingTileTextureID;
    unsigned int podiumTextureID;
    unsigned int clockTextureID;

    Classroom();
    ~Classroom();

    void initializeGeometry();
    void render(Shader& shader);
    void renderLights(Shader& lightShader);
    void updateFan(float deltaTime);
    void updateProjectorScreen(float deltaTime);
    void renderFan(Shader& shader);
    void renderPodium(Shader& shader);
    void renderBenches(Shader& shader);
    void renderDoor(Shader& shader);
    void renderProjector(Shader& shader);
    void renderClock(Shader& shader);
    void toggleFan(int fanIndex);  // Toggle fan on/off
    void toggleProjectorScreen();  // Toggle projector screen up/down

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
    void generateClock();

    void addQuad(std::vector<float>& vertices, 
                glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, 
                glm::vec3 normal, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4);
    
    void addCube(std::vector<float>& vertices, 
                glm::vec3 position, glm::vec3 size);

    void setupBuffers(unsigned int& VAO, unsigned int& VBO, const std::vector<float>& vertices);
    void renderBuffer(unsigned int VAO, size_t vertexCount, const glm::vec3& materialAmbient, 
                     const glm::vec3& materialDiffuse, const glm::vec3& materialSpecular, 
                     Shader& shader);
    unsigned int loadTexture(const char* path);
};

#endif