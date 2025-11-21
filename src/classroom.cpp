#include "../include/classroom.h"
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

Classroom::Classroom()
{
    // Constructor - buffers will be initialized in initializeGeometry()
    fanRotation = 0.0f;
    // Initialize all fans to running state
    for (int i = 0; i < 6; i++)
    {
        fanRunning[i] = true;
    }
    
    // Initialize projector screen state
    screenExtension = 0.0f;    // Start rolled up
    screenRolling = true;
    screenExtending = true;
}

Classroom::~Classroom()
{
    // Clean up OpenGL resources
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &ceilingVAO);
    glDeleteBuffers(1, &ceilingVBO);
    glDeleteVertexArrays(1, &wallsVAO);
    glDeleteBuffers(1, &wallsVBO);
    glDeleteVertexArrays(1, &doorsVAO);
    glDeleteBuffers(1, &doorsVBO);
    glDeleteVertexArrays(1, &windowsVAO);
    glDeleteBuffers(1, &windowsVBO);
    glDeleteVertexArrays(1, &benchesVAO);
    glDeleteBuffers(1, &benchesVBO);
    glDeleteVertexArrays(1, &podiumVAO);
    glDeleteBuffers(1, &podiumVBO);
    glDeleteVertexArrays(1, &boardVAO);
    glDeleteBuffers(1, &boardVBO);
    glDeleteVertexArrays(1, &boardBorderVAO);
    glDeleteBuffers(1, &boardBorderVBO);
    glDeleteVertexArrays(1, &lightsVAO);
    glDeleteBuffers(1, &lightsVBO);
}

void Classroom::initializeGeometry()
{
    // Generate all geometry
    generateFloor();
    generateCeiling();
    generateWalls();
    generateDoors();
    generateWindows();
    generateBenches();
    generatePodium();
    generateGreenBoard();
    generateLights();

    // Setup all buffers
    setupBuffers(floorVAO, floorVBO, floorVertices);
    setupBuffers(ceilingVAO, ceilingVBO, ceilingVertices);
    setupBuffers(ceilingSlabVAO, ceilingSlabVBO, ceilingSlabVertices);
    setupBuffers(ceilingTilesVAO, ceilingTilesVBO, ceilingTilesVertices);
    setupBuffers(wallsVAO, wallsVBO, wallVertices);
    setupBuffers(doorsVAO, doorsVBO, doorVertices);
    setupBuffers(windowsVAO, windowsVBO, windowVertices);
    setupBuffers(benchesVAO, benchesVBO, benchVertices);
    setupBuffers(podiumVAO, podiumVBO, podiumVertices);
    setupBuffers(boardVAO, boardVBO, boardVertices);
    setupBuffers(boardBorderVAO, boardBorderVBO, boardBorderVertices);
    setupBuffers(lightsVAO, lightsVBO, lightVertices);
    
    // Load OBJ models
    if (!fanModel.loadOBJ("models/fan_up.obj"))
    {
        std::cout << "Warning: Failed to load fan model. Please place fan_up.obj in models/ directory" << std::endl;
    }
    
    if (!podiumModel.loadOBJ("models/podium_v4.obj"))
    {
        std::cout << "Warning: Failed to load podium model. Please place podium.obj in models/ directory" << std::endl;
    }
    
    if (!benchModel.loadOBJ("models/classroom_desk.obj"))
    {
        std::cout << "Warning: Failed to load bench model. Please place bench.obj in models/ directory" << std::endl;
    }
    
    if (!projectorModel.loadOBJ("models/projectorrod.obj"))
    {
        std::cout << "Warning: Failed to load projector model. Please place projectorrod.obj in models/ directory" << std::endl;
    }
    
    if (!projectorV2Model.loadOBJ("models/projector_v2.obj"))
    {
        std::cout << "Warning: Failed to load projector_v2 model. Please place projector_v2.obj in models/ directory" << std::endl;
    }
    
    // Load textures
    benchTextureID = loadTexture("textures/bench_texture_v3.jpg");
    ceilingTileTextureID = loadTexture("textures/tile_texture.jpeg");
    podiumTextureID = loadTexture("textures/podium_texture_v2.jpg");
}

void Classroom::generateFloor()
{
    floorVertices.clear();
    
    // Create floor as a large quad
    glm::vec3 v1(-ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 v2(ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 v3(ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 v4(-ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    
    addQuad(floorVertices, v1, v2, v3, v4, normal,
           glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 0.0f), 
           glm::vec2(4.0f, 3.0f), glm::vec2(0.0f, 3.0f));
}

void Classroom::generateCeiling()
{
    ceilingVertices.clear();
    ceilingSlabVertices.clear();
    ceilingTilesVertices.clear();
    
    // First, create solid ceiling slab with thickness that rests on walls
    float ceilingThickness = 0.2f;  // Thickness of the solid ceiling slab
    float tileGap = 0.01f;          // Gap between solid ceiling bottom and tiles
    
    // Ceiling slab dimensions (extends to wall edges)
    float ceilingWidth = ROOM_WIDTH;
    float ceilingLength = ROOM_LENGTH;
    float ceilingTop = ROOM_HEIGHT + ceilingThickness;
    float ceilingBottom = ROOM_HEIGHT;
    
    // Bottom face of ceiling slab (visible from inside room)
    glm::vec3 normalDown(0.0f, -1.0f, 0.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(-ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingBottom, ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingBottom, ceilingLength/2),
           normalDown,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Top face of ceiling slab
    glm::vec3 normalUp(0.0f, 1.0f, 0.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(-ceilingWidth/2, ceilingTop, ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, -ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingTop, -ceilingLength/2),
           normalUp,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Front edge (along width, front side)
    glm::vec3 normalFront(0.0f, 0.0f, 1.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(-ceilingWidth/2, ceilingBottom, ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingBottom, ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingTop, ceilingLength/2),
           normalFront,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Back edge (along width, back side)
    glm::vec3 normalBack(0.0f, 0.0f, -1.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingTop, -ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, -ceilingLength/2),
           normalBack,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Left edge (along length)
    glm::vec3 normalLeft(-1.0f, 0.0f, 0.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(-ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingBottom, ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingTop, ceilingLength/2),
           glm::vec3(-ceilingWidth/2, ceilingTop, -ceilingLength/2),
           normalLeft,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Right edge (along length)
    glm::vec3 normalRight(1.0f, 0.0f, 0.0f);
    addQuad(ceilingSlabVertices,
           glm::vec3(ceilingWidth/2, ceilingBottom, ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingBottom, -ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, -ceilingLength/2),
           glm::vec3(ceilingWidth/2, ceilingTop, ceilingLength/2),
           normalRight,
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Then create ceiling tiles below the solid ceiling
    // Calculate number of tiles to fit the room completely
    int numTilesX = 20;  // Number of tiles across width
    int numTilesZ = 16;  // Number of tiles across length
    
    // Calculate tile size to cover room completely
    float tileSizeX = ROOM_WIDTH / numTilesX;
    float tileSizeZ = ROOM_LENGTH / numTilesZ;
    
    float gap = 0.01f;      // Small gap between tiles for visible grid lines
    
    float startX = -ROOM_WIDTH / 2.0f;
    float startZ = -ROOM_LENGTH / 2.0f;
    
    // Position tiles below the solid ceiling slab
    float tileHeight = ROOM_HEIGHT - tileGap;
    
    glm::vec3 normal(0.0f, -1.0f, 0.0f);
    
    for (int i = 0; i < numTilesX; i++)
    {
        for (int j = 0; j < numTilesZ; j++)
        {
            float x1 = startX + i * tileSizeX + gap;
            float x2 = startX + (i + 1) * tileSizeX - gap;
            float z1 = startZ + j * tileSizeZ + gap;
            float z2 = startZ + (j + 1) * tileSizeZ - gap;
            
            glm::vec3 v1(x1, tileHeight, z1);
            glm::vec3 v2(x2, tileHeight, z1);
            glm::vec3 v3(x2, tileHeight, z2);
            glm::vec3 v4(x1, tileHeight, z2);
            
            addQuad(ceilingTilesVertices, v1, v2, v3, v4, normal,
                   glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
                   glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
        }
    }
}

void Classroom::generateWalls()
{
    wallVertices.clear();
    
    // Window parameters (shared for left and front walls)
    float windowWidth = 1.8f;
    float windowHeight = 0.6f;
    float ceilingGap = 0.1f;
    float spacing = 0.2f;
    float sideGap = 0.1f;  // Gap from wall edge
    
    // Window position (near ceiling)
    float windowTopY = ROOM_HEIGHT - ceilingGap;
    float windowBottomY = windowTopY - windowHeight;
    
    // Front wall (where green board is) - with SIX windows cut out in a single row
    // Calculate window positions along X-axis (left to right)
    // First window
    float frontWindow1X = -ROOM_WIDTH/2 + sideGap + windowWidth/2;
    float frontWindow1StartX = frontWindow1X - windowWidth/2;
    float frontWindow1EndX = frontWindow1X + windowWidth/2;
    
    // Second window
    float frontWindow2X = frontWindow1EndX + spacing + windowWidth/2;
    float frontWindow2StartX = frontWindow2X - windowWidth/2;
    float frontWindow2EndX = frontWindow2X + windowWidth/2;
    
    // Third window
    float frontWindow3X = frontWindow2EndX + spacing + windowWidth/2;
    float frontWindow3StartX = frontWindow3X - windowWidth/2;
    float frontWindow3EndX = frontWindow3X + windowWidth/2;
    
    // Fourth window
    float frontWindow4X = frontWindow3EndX + spacing + windowWidth/2;
    float frontWindow4StartX = frontWindow4X - windowWidth/2;
    float frontWindow4EndX = frontWindow4X + windowWidth/2;
    
    // Fifth window
    float frontWindow5X = frontWindow4EndX + spacing + windowWidth/2;
    float frontWindow5StartX = frontWindow5X - windowWidth/2;
    float frontWindow5EndX = frontWindow5X + windowWidth/2;
    
    // Sixth window
    float frontWindow6X = frontWindow5EndX + spacing + windowWidth/2;
    float frontWindow6StartX = frontWindow6X - windowWidth/2;
    float frontWindow6EndX = frontWindow6X + windowWidth/2;
    
    // Front wall section 1: From left corner to first window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow1StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow1StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // FIRST WINDOW
    // Below first window
    addQuad(wallVertices,
           glm::vec3(frontWindow1StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow1EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow1EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow1StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above first window
    addQuad(wallVertices,
           glm::vec3(frontWindow1StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow1EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow1EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow1StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Between first and second window
    addQuad(wallVertices,
           glm::vec3(frontWindow1EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow2StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow2StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow1EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // SECOND WINDOW
    // Below second window
    addQuad(wallVertices,
           glm::vec3(frontWindow2StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow2EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow2EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow2StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above second window
    addQuad(wallVertices,
           glm::vec3(frontWindow2StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow2EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow2EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow2StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Between second and third window
    addQuad(wallVertices,
           glm::vec3(frontWindow2EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow3StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow3StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow2EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // THIRD WINDOW
    // Below third window
    addQuad(wallVertices,
           glm::vec3(frontWindow3StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow3EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow3EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow3StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above third window
    addQuad(wallVertices,
           glm::vec3(frontWindow3StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow3EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow3EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow3StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Between third and fourth window
    addQuad(wallVertices,
           glm::vec3(frontWindow3EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow4StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow4StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow3EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // FOURTH WINDOW
    // Below fourth window
    addQuad(wallVertices,
           glm::vec3(frontWindow4StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow4EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow4EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow4StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above fourth window
    addQuad(wallVertices,
           glm::vec3(frontWindow4StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow4EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow4EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow4StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Between fourth and fifth window
    addQuad(wallVertices,
           glm::vec3(frontWindow4EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow5StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow5StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow4EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // FIFTH WINDOW
    // Below fifth window
    addQuad(wallVertices,
           glm::vec3(frontWindow5StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow5EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow5EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow5StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above fifth window
    addQuad(wallVertices,
           glm::vec3(frontWindow5StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow5EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow5EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow5StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Between fifth and sixth window
    addQuad(wallVertices,
           glm::vec3(frontWindow5EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow6StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow6StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow5EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // SIXTH WINDOW
    // Below sixth window
    addQuad(wallVertices,
           glm::vec3(frontWindow6StartX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow6EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(frontWindow6EndX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(frontWindow6StartX, windowBottomY, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Above sixth window
    addQuad(wallVertices,
           glm::vec3(frontWindow6StartX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow6EndX, windowTopY, ROOM_LENGTH/2),
           glm::vec3(frontWindow6EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow6StartX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.1f));
    
    // Front wall section: From sixth window to right corner
    addQuad(wallVertices,
           glm::vec3(frontWindow6EndX, 0.0f, ROOM_LENGTH/2),
           glm::vec3(ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2),
           glm::vec3(ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(frontWindow6EndX, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Back wall (entrance side)
    glm::vec3 back_v1(ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 back_v2(-ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 back_v3(-ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    glm::vec3 back_v4(ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    
    addQuad(wallVertices, back_v1, back_v2, back_v3, back_v4, 
           glm::vec3(0.0f, 0.0f, 1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(6.0f, 0.0f), 
           glm::vec2(6.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Left wall (with door opening and three windows cut out)
    float doorWidth = 1.0f;
    float doorHeight = 2.1f;
    float doorZ = -ROOM_LENGTH/2 + 1.5f; // Near the front wall (1.5m from front)
    float doorStartZ = doorZ - doorWidth/2;
    float doorEndZ = doorZ + doorWidth/2;
    
    // Three windows on front side of left wall (after the door)
    // Using shared window parameters declared at function top
    
    // Third window (closest to front wall)
    float window3Z = ROOM_LENGTH/2 - sideGap - windowWidth/2;
    float window3StartZ = window3Z - windowWidth/2;
    float window3EndZ = window3Z + windowWidth/2;
    
    // Second window
    float window2Z = window3StartZ - spacing - windowWidth/2;
    float window2StartZ = window2Z - windowWidth/2;
    float window2EndZ = window2Z + windowWidth/2;
    
    // First window
    float window1Z = window2StartZ - spacing - windowWidth/2;
    float window1StartZ = window1Z - windowWidth/2;
    float window1EndZ = window1Z + windowWidth/2;
    
    // Section 1: From back corner to start of door opening
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, doorStartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, doorStartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 0.0f),
           glm::vec2(2.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Section 2: Above door opening
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, doorHeight, doorStartZ),
           glm::vec3(-ROOM_WIDTH/2, doorHeight, doorEndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, doorEndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, doorStartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 3: From end of door to start of first window (full height)
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, doorEndZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window1StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window1StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, doorEndZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Section 4: Below first window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window1StartZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window1EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window1EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window1StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 5: Above first window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window1StartZ),
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window1EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window1EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window1StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 6: Between first and second window (full height)
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window1EndZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window2StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window2StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window1EndZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.5f, 0.0f),
           glm::vec2(0.5f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Section 7: Below second window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window2StartZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window2EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window2EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window2StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 8: Above second window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window2StartZ),
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window2EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window2EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window2StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 9: Between second and third window (full height)
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window2EndZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window3StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window3StartZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window2EndZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.0f),
           glm::vec2(0.2f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Section 10: Below third window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window3StartZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window3EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window3EndZ),
           glm::vec3(-ROOM_WIDTH/2, windowBottomY, window3StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 11: Above third window
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window3StartZ),
           glm::vec3(-ROOM_WIDTH/2, windowTopY, window3EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window3EndZ),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window3StartZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Section 12: From end of third window to front corner
    addQuad(wallVertices,
           glm::vec3(-ROOM_WIDTH/2, 0.0f, window3EndZ),
           glm::vec3(-ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2),
           glm::vec3(-ROOM_WIDTH/2, ROOM_HEIGHT, window3EndZ),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
           glm::vec2(1.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Door frame border around the opening
    float frameWidth = 0.08f;   // Width of the frame
    float frameDepth = 0.03f;   // How much the frame protrudes from the wall
    
    // EXTERIOR DOOR FRAME (outside the room)
    // Left vertical frame (back side)
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 - frameDepth/2, doorHeight/2, doorStartZ - frameWidth/2),
           glm::vec3(frameDepth, doorHeight, frameWidth));
    
    // Right vertical frame (front side)
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 - frameDepth/2, doorHeight/2, doorEndZ + frameWidth/2),
           glm::vec3(frameDepth, doorHeight, frameWidth));
    
    // Top horizontal frame
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 - frameDepth/2, doorHeight + frameWidth/2, doorZ),
           glm::vec3(frameDepth, frameWidth, doorWidth + 2*frameWidth));
    
    // INTERIOR DOOR FRAME (inside the room)
    float interiorFrameDepth = 0.04f;  // Protrudes into the room
    
    // Left vertical frame (back side) - interior
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 + interiorFrameDepth/2, doorHeight/2, doorStartZ - frameWidth/2),
           glm::vec3(interiorFrameDepth, doorHeight, frameWidth));
    
    // Right vertical frame (front side) - interior
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 + interiorFrameDepth/2, doorHeight/2, doorEndZ + frameWidth/2),
           glm::vec3(interiorFrameDepth, doorHeight, frameWidth));
    
    // Top horizontal frame - interior
    addCube(wallVertices,
           glm::vec3(-ROOM_WIDTH/2 + interiorFrameDepth/2, doorHeight + frameWidth/2, doorZ),
           glm::vec3(interiorFrameDepth, frameWidth, doorWidth + 2*frameWidth));
    
    // Right wall with cuboid in the middle
    glm::vec3 right_v1(ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 right_v2(ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 right_v3(ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    glm::vec3 right_v4(ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    
    addQuad(wallVertices, right_v1, right_v2, right_v3, right_v4, 
           glm::vec3(-1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 0.0f), 
           glm::vec2(4.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Add cuboid on right wall (exactly in the middle)
    float cuboidWidth = 1.0f;   // Width along Z-axis
    float cuboidHeight = 3.6f;  // Height along Y-axis
    float cuboidDepth = 0.3f;   // Depth protruding from wall
    
    // Position in exact middle of wall
    float cuboidX = ROOM_WIDTH/2 - cuboidDepth/2;  // On right wall
    float cuboidY = ROOM_HEIGHT/2;  // Middle height
    float cuboidZ = 0.0f;  // Middle of wall along Z-axis
    
    addCube(wallVertices,
           glm::vec3(cuboidX, cuboidY, cuboidZ),
           glm::vec3(cuboidDepth, cuboidHeight, cuboidWidth));
}

void Classroom::generateDoors()
{
    doorVertices.clear();
    
    // Create door panel that will be rotated when rendered
    float doorWidth = 1.0f;
    float doorHeight = 2.1f;
    float doorThickness = 0.02f;
    
    // Create door centered at origin for easy rotation
    // The door will be positioned and rotated in renderDoor()
    addCube(doorVertices, 
           glm::vec3(0.0f, doorHeight/2, 0.0f), 
           glm::vec3(doorThickness, doorHeight, doorWidth));
}

void Classroom::generateWindows()
{
    windowVertices.clear();
    
    // Window parameters
    float windowWidth = 1.8f;
    float windowHeight = 0.6f;
    float ceilingGap = 0.1f;
    float spacing = 0.2f;
    float sideGap = 0.1f;
    
    // Window position (near ceiling)
    float windowTopY = ROOM_HEIGHT - ceilingGap;
    float windowBottomY = windowTopY - windowHeight;
    float windowCenterY = windowBottomY + windowHeight/2;
    
    // Glass thickness
    float glassThickness = 0.02f;
    
    // Window frame parameters
    float frameWidth = 0.06f;      // Width of the frame border
    float frameDepth = 0.04f;      // How much the frame protrudes into the room
    
    // THREE WINDOWS ON LEFT WALL
    float leftWallX = -ROOM_WIDTH/2 + glassThickness/2;
    
    // Third window (closest to front wall)
    float window3Z = ROOM_LENGTH/2 - sideGap - windowWidth/2;
    addCube(windowVertices,
           glm::vec3(leftWallX, windowCenterY, window3Z),
           glm::vec3(glassThickness, windowHeight, windowWidth));
    
    // Frame for third window on left wall
    float leftFrameX = -ROOM_WIDTH/2 + frameDepth/2;
    // Left vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window3Z - windowWidth/2 - frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Right vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window3Z + windowWidth/2 + frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Top horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowTopY + frameWidth/2, window3Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    // Bottom horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowBottomY - frameWidth/2, window3Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    
    // Second window
    float window2Z = window3Z - windowWidth/2 - spacing - windowWidth/2;
    addCube(windowVertices,
           glm::vec3(leftWallX, windowCenterY, window2Z),
           glm::vec3(glassThickness, windowHeight, windowWidth));
    
    // Frame for second window on left wall
    // Left vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window2Z - windowWidth/2 - frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Right vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window2Z + windowWidth/2 + frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Top horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowTopY + frameWidth/2, window2Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    // Bottom horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowBottomY - frameWidth/2, window2Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    
    // First window
    float window1Z = window2Z - windowWidth/2 - spacing - windowWidth/2;
    addCube(windowVertices,
           glm::vec3(leftWallX, windowCenterY, window1Z),
           glm::vec3(glassThickness, windowHeight, windowWidth));
    
    // Frame for first window on left wall
    // Left vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window1Z - windowWidth/2 - frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Right vertical frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowCenterY, window1Z + windowWidth/2 + frameWidth/2),
           glm::vec3(frameDepth, windowHeight + 2*frameWidth, frameWidth));
    // Top horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowTopY + frameWidth/2, window1Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    // Bottom horizontal frame
    addCube(wallVertices,
           glm::vec3(leftFrameX, windowBottomY - frameWidth/2, window1Z),
           glm::vec3(frameDepth, frameWidth, windowWidth + 2*frameWidth));
    
    // SIX WINDOWS ON FRONT WALL (single row, side by side)
    float frontWallZ = ROOM_LENGTH/2 - glassThickness/2;
    float frontFrameZ = ROOM_LENGTH/2 - frameDepth/2;
    
    // First window
    float frontWindow1X = -ROOM_WIDTH/2 + sideGap + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow1X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for first window on front wall
    // Left vertical frame
    addCube(wallVertices,
           glm::vec3(frontWindow1X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    // Right vertical frame
    addCube(wallVertices,
           glm::vec3(frontWindow1X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    // Top horizontal frame
    addCube(wallVertices,
           glm::vec3(frontWindow1X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    // Bottom horizontal frame
    addCube(wallVertices,
           glm::vec3(frontWindow1X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    
    // Second window
    float frontWindow2X = frontWindow1X + windowWidth/2 + spacing + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow2X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for second window on front wall
    addCube(wallVertices,
           glm::vec3(frontWindow2X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow2X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow2X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow2X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    
    // Third window
    float frontWindow3X = frontWindow2X + windowWidth/2 + spacing + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow3X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for third window on front wall
    addCube(wallVertices,
           glm::vec3(frontWindow3X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow3X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow3X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow3X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    
    // Fourth window
    float frontWindow4X = frontWindow3X + windowWidth/2 + spacing + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow4X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for fourth window on front wall
    addCube(wallVertices,
           glm::vec3(frontWindow4X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow4X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow4X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow4X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    
    // Fifth window
    float frontWindow5X = frontWindow4X + windowWidth/2 + spacing + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow5X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for fifth window on front wall
    addCube(wallVertices,
           glm::vec3(frontWindow5X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow5X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow5X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow5X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    
    // Sixth window
    float frontWindow6X = frontWindow5X + windowWidth/2 + spacing + windowWidth/2;
    addCube(windowVertices,
           glm::vec3(frontWindow6X, windowCenterY, frontWallZ),
           glm::vec3(windowWidth, windowHeight, glassThickness));
    
    // Frame for sixth window on front wall
    addCube(wallVertices,
           glm::vec3(frontWindow6X - windowWidth/2 - frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow6X + windowWidth/2 + frameWidth/2, windowCenterY, frontFrameZ),
           glm::vec3(frameWidth, windowHeight + 2*frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow6X, windowTopY + frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
    addCube(wallVertices,
           glm::vec3(frontWindow6X, windowBottomY - frameWidth/2, frontFrameZ),
           glm::vec3(windowWidth + 2*frameWidth, frameWidth, frameDepth));
}

void Classroom::generateBenches()
{

}



void Classroom::generatePodium()
{
    
}

void Classroom::generateGreenBoard()
{
    boardVertices.clear();
    boardBorderVertices.clear();
    
    // Two green boards on the front wall - side by side
    float boardWidth = 4.3f;  // Width of each board
    float boardHeight = 1.5f; // Height of boards
    float boardY = 1.8f;      // Vertical position
    float boardThickness = 0.08f; // Thickness
    float spacing = 0.04f;     // Space between the two boards
    float borderThickness = 0.05f; // Border frame thickness (reduced from 0.1f)
    float borderDepth = 0.03f;    // How much border protrudes (reduced from 0.04f)
    
    // Left board
    addCube(boardVertices, 
           glm::vec3(-boardWidth/2 - spacing/2, boardY, -ROOM_LENGTH/2 + boardThickness/2), 
           glm::vec3(boardWidth, boardHeight, boardThickness));
    
    // Left board silver border (4 pieces: left, right, top, bottom)
    float leftBoardX = -boardWidth/2 - spacing/2;
    // Left vertical border
    addCube(boardBorderVertices,
           glm::vec3(leftBoardX - boardWidth/2 - borderThickness/2, boardY, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(borderThickness, boardHeight + 2*borderThickness, borderDepth));
    // Right vertical border
    addCube(boardBorderVertices,
           glm::vec3(leftBoardX + boardWidth/2 + borderThickness/2, boardY, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(borderThickness, boardHeight + 2*borderThickness, borderDepth));
    // Top horizontal border
    addCube(boardBorderVertices,
           glm::vec3(leftBoardX, boardY + boardHeight/2 + borderThickness/2, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(boardWidth, borderThickness, borderDepth));
    // Bottom horizontal border
    addCube(boardBorderVertices,
           glm::vec3(leftBoardX, boardY - boardHeight/2 - borderThickness/2, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(boardWidth, borderThickness, borderDepth));
    
    // Right board
    addCube(boardVertices, 
           glm::vec3(boardWidth/2 + spacing/2, boardY, -ROOM_LENGTH/2 + boardThickness/2), 
           glm::vec3(boardWidth, boardHeight, boardThickness));
    
    // Right board silver border (4 pieces: left, right, top, bottom)
    float rightBoardX = boardWidth/2 + spacing/2;
    // Left vertical border
    addCube(boardBorderVertices,
           glm::vec3(rightBoardX - boardWidth/2 - borderThickness/2, boardY, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(borderThickness, boardHeight + 2*borderThickness, borderDepth));
    // Right vertical border
    addCube(boardBorderVertices,
           glm::vec3(rightBoardX + boardWidth/2 + borderThickness/2, boardY, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(borderThickness, boardHeight + 2*borderThickness, borderDepth));
    // Top horizontal border
    addCube(boardBorderVertices,
           glm::vec3(rightBoardX, boardY + boardHeight/2 + borderThickness/2, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(boardWidth, borderThickness, borderDepth));
    // Bottom horizontal border
    addCube(boardBorderVertices,
           glm::vec3(rightBoardX, boardY - boardHeight/2 - borderThickness/2, -ROOM_LENGTH/2 + boardThickness + borderDepth/2),
           glm::vec3(boardWidth, borderThickness, borderDepth));
}

void Classroom::generateLights()
{
    lightVertices.clear();
    
    // 4 square ceiling lights at corners to match the light sources
    float lightSize = 1.6f;     // Square light panel size
    float lightHeight = 0.08f;  // Thin panel light
    float cornerOffset = 1.8f;  // Distance from corners (matching light source positions)
    
    // Front-left corner light
    addCube(lightVertices,
           glm::vec3(-ROOM_WIDTH/2 + cornerOffset, ROOM_HEIGHT - lightHeight/2, -ROOM_LENGTH/2 + cornerOffset),
           glm::vec3(lightSize, lightHeight, lightSize));
    
    // Front-right corner light
    addCube(lightVertices,
           glm::vec3(ROOM_WIDTH/2 - cornerOffset, ROOM_HEIGHT - lightHeight/2, -ROOM_LENGTH/2 + cornerOffset),
           glm::vec3(lightSize, lightHeight, lightSize));
    
    // Back-left corner light
    addCube(lightVertices,
           glm::vec3(-ROOM_WIDTH/2 + cornerOffset, ROOM_HEIGHT - lightHeight/2, ROOM_LENGTH/2 - cornerOffset),
           glm::vec3(lightSize, lightHeight, lightSize));
    
    // Back-right corner light
    addCube(lightVertices,
           glm::vec3(ROOM_WIDTH/2 - cornerOffset, ROOM_HEIGHT - lightHeight/2, ROOM_LENGTH/2 - cornerOffset),
           glm::vec3(lightSize, lightHeight, lightSize));
}

void Classroom::addQuad(std::vector<float>& vertices, 
                       glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, 
                       glm::vec3 normal, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4)
{
    // First triangle (v1, v2, v3)
    // Vertex 1
    vertices.insert(vertices.end(), {v1.x, v1.y, v1.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv1.x, uv1.y});
    
    // Vertex 2
    vertices.insert(vertices.end(), {v2.x, v2.y, v2.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv2.x, uv2.y});
    
    // Vertex 3
    vertices.insert(vertices.end(), {v3.x, v3.y, v3.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv3.x, uv3.y});
    
    // Second triangle (v1, v3, v4)
    // Vertex 1
    vertices.insert(vertices.end(), {v1.x, v1.y, v1.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv1.x, uv1.y});
    
    // Vertex 3
    vertices.insert(vertices.end(), {v3.x, v3.y, v3.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv3.x, uv3.y});
    
    // Vertex 4
    vertices.insert(vertices.end(), {v4.x, v4.y, v4.z});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
    vertices.insert(vertices.end(), {uv4.x, uv4.y});
}

void Classroom::addCube(std::vector<float>& vertices, glm::vec3 position, glm::vec3 size)
{
    float x = position.x, y = position.y, z = position.z;
    float w = size.x / 2.0f, h = size.y / 2.0f, d = size.z / 2.0f;
    
    // Front face
    addQuad(vertices, 
           glm::vec3(x-w, y-h, z+d), glm::vec3(x+w, y-h, z+d), 
           glm::vec3(x+w, y+h, z+d), glm::vec3(x-w, y+h, z+d),
           glm::vec3(0.0f, 0.0f, 1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Back face
    addQuad(vertices, 
           glm::vec3(x+w, y-h, z-d), glm::vec3(x-w, y-h, z-d), 
           glm::vec3(x-w, y+h, z-d), glm::vec3(x+w, y+h, z-d),
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Left face
    addQuad(vertices, 
           glm::vec3(x-w, y-h, z-d), glm::vec3(x-w, y-h, z+d), 
           glm::vec3(x-w, y+h, z+d), glm::vec3(x-w, y+h, z-d),
           glm::vec3(-1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Right face
    addQuad(vertices, 
           glm::vec3(x+w, y-h, z+d), glm::vec3(x+w, y-h, z-d), 
           glm::vec3(x+w, y+h, z-d), glm::vec3(x+w, y+h, z+d),
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Top face
    addQuad(vertices, 
           glm::vec3(x-w, y+h, z+d), glm::vec3(x+w, y+h, z+d), 
           glm::vec3(x+w, y+h, z-d), glm::vec3(x-w, y+h, z-d),
           glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    // Bottom face
    addQuad(vertices, 
           glm::vec3(x-w, y-h, z-d), glm::vec3(x+w, y-h, z-d), 
           glm::vec3(x+w, y-h, z+d), glm::vec3(x-w, y-h, z+d),
           glm::vec3(0.0f, -1.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 
           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
}

void Classroom::setupBuffers(unsigned int& VAO, unsigned int& VBO, const std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Classroom::renderBuffer(unsigned int VAO, size_t vertexCount, 
                           const glm::vec3& materialAmbient, 
                           const glm::vec3& materialDiffuse, 
                           const glm::vec3& materialSpecular, 
                           Shader& shader)
{
    shader.setVec3("material.ambient", materialAmbient);
    shader.setVec3("material.diffuse", materialDiffuse);
    shader.setVec3("material.specular", materialSpecular);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Classroom::render(Shader& shader)
{
    // Disable textures by default (enabled only for specific objects)
    shader.setBool("useWoodTexture", false);
    shader.setBool("useCeilingTiles", false);
    shader.setBool("useFloorTiles", false);
    
    // Render floor (gray tiles with tile texture)
    shader.setBool("useFloorTiles", true);  // Enable floor tile texture
    renderBuffer(floorVAO, floorVertices.size() / 8, 
    glm::vec3(0.8f, 0.8f, 0.8f),   // Ambient - soft white
    glm::vec3(0.95f, 0.95f, 0.95f), // Diffuse - bright white
    glm::vec3(0.6f, 0.6f, 0.6f),   // Specular - adds slight shine
    shader);
    shader.setBool("useFloorTiles", false);  // Disable for other objects

    
    // Render solid ceiling slab (white, no texture)
    renderBuffer(ceilingSlabVAO, ceilingSlabVertices.size() / 8, 
    glm::vec3(0.95f, 0.95f, 0.95f),   // Ambient - bright white
    glm::vec3(1.0f, 1.0f, 1.0f),      // Diffuse - pure white
    glm::vec3(0.3f, 0.3f, 0.3f),      // Specular - slight shine
    shader);
    
    // Render ceiling tiles (with image texture)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ceilingTileTextureID);
    shader.setInt("ceilingTileTexture", 0);
    shader.setBool("useCeilingTileTexture", true);  // Enable ceiling tile image texture
    renderBuffer(ceilingTilesVAO, ceilingTilesVertices.size() / 8, 
    glm::vec3(0.9f, 0.9f, 0.9f),   // Ambient - almost pure white
    glm::vec3(1.0f, 1.0f, 1.0f),   // Diffuse - perfect white under light
    glm::vec3(0.3f, 0.3f, 0.3f),   // Specular - slight shine to reflect light naturally
    shader);
    shader.setBool("useCeilingTileTexture", false);  // Disable for other objects

    
    // Render walls (light beige - no texture)
    renderBuffer(wallsVAO, wallVertices.size() / 8, 
                glm::vec3(0.8f, 0.75f, 0.65f), 
                glm::vec3(0.9f, 0.85f, 0.75f), 
                glm::vec3(0.1f, 0.1f, 0.1f), shader);
    

       // Render door (open at an angle)
       renderDoor(shader);
    
    // Render windows with transparency
    if (windowVertices.size() > 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Blue-tinted transparent glass
        shader.setVec3("material.ambient", glm::vec3(0.1f, 0.15f, 0.2f));
        shader.setVec3("material.diffuse", glm::vec3(0.2f, 0.3f, 0.4f));
        shader.setVec3("material.specular", glm::vec3(0.6f, 0.7f, 0.8f));
        shader.setFloat("material.shininess", 32.0f);
        shader.setFloat("alpha", 0.7f);  // 70% opacity
        
        glBindVertexArray(windowsVAO);
        glDrawArrays(GL_TRIANGLES, 0, windowVertices.size() / 8);
        
        shader.setFloat("alpha", 1.0f);  // Reset alpha
        glDisable(GL_BLEND);
    }

       // Render benches from OBJ model if available, otherwise use procedural geometry
       if (!benchModel.vertices.empty())
       {
           renderBenches(shader);
       }
       else
       {
           renderBuffer(benchesVAO, benchVertices.size() / 8,
           glm::vec3(0.35f, 0.20f, 0.07f),   // ambient - darker base
           glm::vec3(0.65f, 0.40f, 0.15f),   // diffuse - rich teak color
           glm::vec3(0.25f, 0.18f, 0.10f),   // specular - slight shine
           shader);
       }


    
    // Render podium from OBJ model if available, otherwise use procedural geometry
    if (!podiumModel.vertices.empty())
    {
        renderPodium(shader);
    }
    else
    {
        renderBuffer(podiumVAO, podiumVertices.size() / 8, 
                    glm::vec3(0.2f, 0.15f, 0.1f), 
                    glm::vec3(0.4f, 0.3f, 0.2f), 
                    glm::vec3(0.15f, 0.1f, 0.08f), shader);
    }
    
    // Render green boards with procedural texture
    shader.setBool("useBoardTexture", true);
    renderBuffer(boardVAO, boardVertices.size() / 8,
    glm::vec3(0.02f, 0.08f, 0.02f),   // ambient - very dark blackish-green
    glm::vec3(0.05f, 0.15f, 0.05f),   // diffuse - dark blackish-green
    glm::vec3(0.03f, 0.08f, 0.03f),   // specular - minimal highlights
    shader);
    shader.setBool("useBoardTexture", false);  // Disable for other objects
    
    // Render green board borders (silver color)
    renderBuffer(boardBorderVAO, boardBorderVertices.size() / 8,
    glm::vec3(0.6f, 0.6f, 0.6f),      // ambient - silver gray
    glm::vec3(0.75f, 0.75f, 0.75f),   // diffuse - bright silver
    glm::vec3(0.9f, 0.9f, 0.9f),      // specular - shiny metallic
    shader);
    
    // Render projector screen
    renderProjector(shader);
}

void Classroom::renderLights(Shader& lightShader)
{
    // Set bright light color to make light sources appear emissive
    lightShader.setVec3("lightColor", glm::vec3(2.5f, 2.5f, 2.2f));  // Much brighter, slightly warm
    
    // Render light fixtures
    glBindVertexArray(lightsVAO);
    glDrawArrays(GL_TRIANGLES, 0, lightVertices.size() / 8);
}
void Classroom::updateFan(float deltaTime)
{
    // Rotate fan at 360 degrees per second (adjust speed as needed)
    fanRotation += 360.0f * deltaTime;
    if (fanRotation > 360.0f)
        fanRotation -= 360.0f;
}

void Classroom::toggleFan(int fanIndex)
{
    if (fanIndex >= 0 && fanIndex < 6)
    {
        fanRunning[fanIndex] = !fanRunning[fanIndex];
        std::cout << "Fan " << (fanIndex + 1) << " is now " 
                  << (fanRunning[fanIndex] ? "ON" : "OFF") << std::endl;
    }
}

void Classroom::toggleProjectorScreen()
{
    if (!screenRolling)
    {
        screenRolling = true;
        // Toggle direction based on current state
        if (screenExtension >= 1.0f)
        {
            screenExtending = false;  // Roll up
            std::cout << "Projector screen rolling UP" << std::endl;
        }
        else if (screenExtension <= 0.0f)
        {
            screenExtending = true;   // Roll down
            std::cout << "Projector screen rolling DOWN" << std::endl;
        }
        else
        {
            // If partially extended, continue in current direction
            std::cout << "Projector screen " << (screenExtending ? "rolling DOWN" : "rolling UP") << std::endl;
        }
    }
}

void Classroom::updateProjectorScreen(float deltaTime)
{
    if (screenRolling)
    {
        float rollSpeed = 0.5f;  // Speed of screen rolling (units per second)
        
        if (screenExtending)
        {
            screenExtension += rollSpeed * deltaTime;
            if (screenExtension >= 1.0f)
            {
                screenExtension = 1.0f;
                screenRolling = false;
                std::cout << "Projector screen fully EXTENDED" << std::endl;
            }
        }
        else
        {
            screenExtension -= rollSpeed * deltaTime;
            if (screenExtension <= 0.0f)
            {
                screenExtension = 0.0f;
                screenRolling = false;
                std::cout << "Projector screen fully ROLLED UP" << std::endl;
            }
        }
    }
}

void Classroom::renderFan(Shader& shader)
{
    if (fanModel.vertices.empty())
        return;  // Fan model not loaded
    
    // Set fan material properties (bright cream/white color for visibility)
    shader.setVec3("material.ambient", glm::vec3(0.6f, 0.55f, 0.5f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.85f, 0.75f));
    shader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    shader.setFloat("material.shininess", 32.0f);
    
    // Render 6 fans in 2 rows x 3 columns
    float fanHeight = ROOM_HEIGHT - 0.6f;  // Position fans at ceiling height
    float fanScale = 0.2f;
    
    // Row 1 (back row)
    float row1Z = -1.5f;
    // Row 2 (front row)
    float row2Z = 1.5f;
    
    // Column positions
    float col1X = -3.5f;  // Left
    float col2X = 0.0f;   // Center
    float col3X = 3.5f;   // Right
    
    // Row 1 - Fan 1 (left) - index 0
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col1X, fanHeight, row1Z));
    if (fanRunning[0]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Row 1 - Fan 2 (center) - index 1
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col2X, fanHeight, row1Z));
    if (fanRunning[1]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Row 1 - Fan 3 (right) - index 2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col3X, fanHeight, row1Z));
    if (fanRunning[2]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Row 2 - Fan 4 (left) - index 3
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col1X, fanHeight, row2Z));
    if (fanRunning[3]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Row 2 - Fan 5 (center) - index 4
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col2X, fanHeight, row2Z));
    if (fanRunning[4]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Row 2 - Fan 6 (right) - index 5
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(col3X, fanHeight, row2Z));
    if (fanRunning[5]) model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fanScale, fanScale, fanScale));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Reset model matrix
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}

void Classroom::renderPodium(Shader& shader)
{
    if (podiumModel.vertices.empty())
        return;  // Podium model not loaded
    
    // Bind and enable podium image texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, podiumTextureID);
    shader.setInt("podiumTexture", 0);
    shader.setBool("usePodiumTexture", true);
    
    // Set podium material properties (dark wood)
    shader.setVec3("material.ambient", glm::vec3(0.2f, 0.15f, 0.1f));
    shader.setVec3("material.diffuse", glm::vec3(0.4f, 0.3f, 0.2f));
    shader.setVec3("material.specular", glm::vec3(0.15f, 0.1f, 0.08f));
    shader.setFloat("material.shininess", 32.0f);
    
    // Position podium on the right side of the green board - on the floor
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.2f, 0.0f, -ROOM_LENGTH/2 + 1.2f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate 90° so opening faces green board
    model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));  // Scale to 20% of original size (scaled up from 10%)
    
    shader.setMat4("model", model);
    
    // Render the podium
    podiumModel.render();
    
    // Disable podium texture after rendering
    shader.setBool("usePodiumTexture", false);
    
    // Reset model matrix
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}

void Classroom::renderBenches(Shader& shader)
{
    if (benchModel.vertices.empty())
        return;  // Bench model not loaded
    
    // Enable bench image texture
    shader.setBool("useBenchTexture", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, benchTextureID);
    shader.setInt("benchTexture", 0);
    
    // Set bench material properties
    shader.setVec3("material.ambient", glm::vec3(0.3f, 0.25f, 0.2f));
    shader.setVec3("material.diffuse", glm::vec3(0.5f, 0.4f, 0.35f));
    shader.setVec3("material.specular", glm::vec3(0.3f, 0.25f, 0.2f));
    shader.setFloat("material.shininess", 32.0f);
    
    // Render benches in 4 rows with 5 benches each
    int numRows = 4;
    int benchesPerRow = 5;
    float startZ = -ROOM_LENGTH/2 + 2.5f;  // Moved backward, leaving more space near board
    float rowSpacing = 1.4f;  // Spacing between rows (more space with 4 rows)
    float benchSpacing = 2.3f;  // Spacing between benches in a row
    
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < benchesPerRow; col++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            
            // Position: center benches horizontally, space vertically
            float xPos = -4.8f + col * benchSpacing;
            float zPos = startZ + row * rowSpacing;
            
            model = glm::translate(model, glm::vec3(xPos, 0.0f, zPos));
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Face front
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));  // Reduced scale to fit classroom
            
            shader.setMat4("model", model);
            benchModel.render();
        }
    }
    
    // Disable bench texture for other objects
    shader.setBool("useBenchTexture", false);
    
    // Reset model matrix
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}

void Classroom::renderDoor(Shader& shader)
{
    // Set door material properties (white)
    shader.setVec3("material.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
    shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setFloat("material.shininess", 32.0f);
    
    float doorZ = -ROOM_LENGTH/2 + 1.5f; // Near the front wall (1.5m from front)
    float doorOpenAngle = 0.0f; // Door is open at 45 degrees
    
    // Position door at the hinge point (left edge of door frame)
    glm::mat4 model = glm::mat4(1.0f);
    
    // Step 1: Translate to hinge position (left edge of door opening)
    model = glm::translate(model, glm::vec3(-ROOM_WIDTH/2, 0.0f, doorZ - 0.5f));
    
    // Step 2: Rotate around Y-axis (vertical hinge) to open the door
    model = glm::rotate(model, glm::radians(doorOpenAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Step 3: Translate door so hinge is at the edge (shift by half door width)
    model = glm::translate(model, glm::vec3(0.04f, 0.0f, 0.5f)); // 0.04 is half thickness
    
    shader.setMat4("model", model);
    
    // Render the door
    glBindVertexArray(doorsVAO);
    glDrawArrays(GL_TRIANGLES, 0, doorVertices.size() / 8);
    
    // Reset model matrix
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}

// Helper function to generate cylinder vertices
std::vector<float> generateCylinderVertices(float radius, float height, int segments)
{
    std::vector<float> vertices;
    
    // Generate vertices for the cylinder sides
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float nextTheta = 2.0f * M_PI * float(i + 1) / float(segments);
        
        float x1 = radius * cos(theta);
        float z1 = radius * sin(theta);
        float x2 = radius * cos(nextTheta);
        float z2 = radius * sin(nextTheta);
        
        // Normal for the side (pointing outward)
        float nx1 = cos(theta);
        float nz1 = sin(theta);
        float nx2 = cos(nextTheta);
        float nz2 = sin(nextTheta);
        
        // Triangle 1 (bottom to top)
        // Bottom vertex 1
        vertices.push_back(x1); vertices.push_back(-height/2); vertices.push_back(z1);
        vertices.push_back(nx1); vertices.push_back(0.0f); vertices.push_back(nz1);
        vertices.push_back(float(i) / segments); vertices.push_back(0.0f);
        
        // Top vertex 1
        vertices.push_back(x1); vertices.push_back(height/2); vertices.push_back(z1);
        vertices.push_back(nx1); vertices.push_back(0.0f); vertices.push_back(nz1);
        vertices.push_back(float(i) / segments); vertices.push_back(1.0f);
        
        // Bottom vertex 2
        vertices.push_back(x2); vertices.push_back(-height/2); vertices.push_back(z2);
        vertices.push_back(nx2); vertices.push_back(0.0f); vertices.push_back(nz2);
        vertices.push_back(float(i + 1) / segments); vertices.push_back(0.0f);
        
        // Triangle 2
        // Top vertex 1
        vertices.push_back(x1); vertices.push_back(height/2); vertices.push_back(z1);
        vertices.push_back(nx1); vertices.push_back(0.0f); vertices.push_back(nz1);
        vertices.push_back(float(i) / segments); vertices.push_back(1.0f);
        
        // Top vertex 2
        vertices.push_back(x2); vertices.push_back(height/2); vertices.push_back(z2);
        vertices.push_back(nx2); vertices.push_back(0.0f); vertices.push_back(nz2);
        vertices.push_back(float(i + 1) / segments); vertices.push_back(1.0f);
        
        // Bottom vertex 2
        vertices.push_back(x2); vertices.push_back(-height/2); vertices.push_back(z2);
        vertices.push_back(nx2); vertices.push_back(0.0f); vertices.push_back(nz2);
        vertices.push_back(float(i + 1) / segments); vertices.push_back(0.0f);
    }
    
    // Generate top cap
    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float nextTheta = 2.0f * M_PI * float(i + 1) / float(segments);
        
        float x1 = radius * cos(theta);
        float z1 = radius * sin(theta);
        float x2 = radius * cos(nextTheta);
        float z2 = radius * sin(nextTheta);
        
        // Center of top cap
        vertices.push_back(0.0f); vertices.push_back(height/2); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f); vertices.push_back(0.5f);
        
        // First point on circle
        vertices.push_back(x1); vertices.push_back(height/2); vertices.push_back(z1);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f + 0.5f * cos(theta)); vertices.push_back(0.5f + 0.5f * sin(theta));
        
        // Second point on circle
        vertices.push_back(x2); vertices.push_back(height/2); vertices.push_back(z2);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f + 0.5f * cos(nextTheta)); vertices.push_back(0.5f + 0.5f * sin(nextTheta));
    }
    
    // Generate bottom cap
    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float nextTheta = 2.0f * M_PI * float(i + 1) / float(segments);
        
        float x1 = radius * cos(theta);
        float z1 = radius * sin(theta);
        float x2 = radius * cos(nextTheta);
        float z2 = radius * sin(nextTheta);
        
        // Center of bottom cap
        vertices.push_back(0.0f); vertices.push_back(-height/2); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f); vertices.push_back(0.5f);
        
        // Second point on circle (reversed for correct winding)
        vertices.push_back(x2); vertices.push_back(-height/2); vertices.push_back(z2);
        vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f + 0.5f * cos(nextTheta)); vertices.push_back(0.5f + 0.5f * sin(nextTheta));
        
        // First point on circle
        vertices.push_back(x1); vertices.push_back(-height/2); vertices.push_back(z1);
        vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
        vertices.push_back(0.5f + 0.5f * cos(theta)); vertices.push_back(0.5f + 0.5f * sin(theta));
    }
    
    return vertices;
}

void Classroom::renderProjector(Shader& shader)
{
    if (projectorModel.vertices.empty())
    {
        return; // Model not loaded
    }
    
    // Set projector material properties (light gray/silver for rod and screen)
    shader.setVec3("material.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
    shader.setVec3("material.specular", glm::vec3(0.6f, 0.6f, 0.6f));
    shader.setFloat("material.shininess", 64.0f);
    
    // Green board parameters (matching the generateGreenBoard function)
    float boardHeight = 1.5f; // Height of boards
    float boardY = 1.2f;      // Vertical position of boards
    
    // Position projector rod to rest horizontally along the front wall
    float projectorX = 0.0f;  // Center between the two boards
    float projectorY = boardY + boardHeight/2 + 1.0f;  // Above the boards
    float projectorZ = -ROOM_LENGTH/2 + 0.05f;  // Close to the wall
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(projectorX, projectorY, projectorZ));
    
    // Rotate so the rod rests horizontally along the front wall
    // First rotate 90 degrees around X-axis, then 90 around Z-axis, 
    // then 90 around Y-axis to change from vertical to horizontal
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Scale the projector screen and rod to make it visible
    float scale = 0.08f;  // Increased scale for better visibility
    model = glm::scale(model, glm::vec3(0.06f, scale, scale));
    
    shader.setMat4("model", model);
    projectorModel.render();
    
    // Render hanging projector (projector_v2) if loaded
    if (!projectorV2Model.vertices.empty())
    {
        // Set material properties for the hanging projector (bright white light)
        shader.setVec3("material.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
        shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 128.0f);
        
        // Position the hanging projector from ceiling in center of room
        glm::mat4 projectorV2Model_mat = glm::mat4(1.0f);
        
        // Position: hanging from ceiling in center of room
        float hangingX = 0.5f;  // Center of room
        float hangingY = ROOM_HEIGHT - 0.8f;  // Near ceiling
        float hangingZ = 0.0f;  // Center along Z-axis
        
        projectorV2Model_mat = glm::translate(projectorV2Model_mat, glm::vec3(hangingX, hangingY, hangingZ));
        
        // Rotate projector to face downward/forward (adjust as needed)
        projectorV2Model_mat = glm::rotate(projectorV2Model_mat, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        projectorV2Model_mat = glm::rotate(projectorV2Model_mat, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Scale the projector
        float projectorScale = 0.15f;
        projectorV2Model_mat = glm::scale(projectorV2Model_mat, glm::vec3(projectorScale, projectorScale, projectorScale));
        
        shader.setMat4("model", projectorV2Model_mat);
        projectorV2Model.render();
    }
    
    shader.setMat4("model", model);
    projectorModel.render();
    
    // Render the rolling screen
    if (screenExtension > 0.0f)
    {
        // Overall screen dimensions (including black frame)
        float totalScreenWidth = 3.0f;
        float maxTotalScreenHeight = 2.0f;
        float currentTotalScreenHeight = maxTotalScreenHeight * screenExtension;
        float screenThickness = 0.02f;
        
        // Black frame dimensions
        float topFrameHeight = 0.08f;
        float sideFrameWidth = 0.08f;
        float bottomFrameHeight = 0.2f;  // Extra long bottom frame
        
        // White projection surface dimensions
        float whiteWidth = totalScreenWidth - (2 * sideFrameWidth);
        float whiteHeight = currentTotalScreenHeight - topFrameHeight - bottomFrameHeight;
        
        // Position screen hanging down from the rod
        float screenCenterX = projectorX;
        float screenTopY = projectorY - 0.1f;  // Slightly below rod
        float screenZ = projectorZ + 0.1f;  // Slightly forward from wall
        
        float screenVertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
            // Back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        };
        
        unsigned int screenVAO, screenVBO;
        glGenVertexArrays(1, &screenVAO);
        glGenBuffers(1, &screenVBO);
        
        glBindVertexArray(screenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        // 1. Draw black background (full screen)
        shader.setVec3("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.setVec3("material.diffuse", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setVec3("material.specular", glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setFloat("material.shininess", 16.0f);
        
        glm::mat4 model = glm::mat4(1.0f);
        float screenCenterY = screenTopY - currentTotalScreenHeight / 2.0f;
        model = glm::translate(model, glm::vec3(screenCenterX, screenCenterY, screenZ));
        model = glm::scale(model, glm::vec3(totalScreenWidth, currentTotalScreenHeight, screenThickness));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        
        // 2. Draw white projection surface on top (if screen is extended enough)
        if (whiteHeight > 0.0f)
        {
            shader.setVec3("material.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
            shader.setVec3("material.diffuse", glm::vec3(0.98f, 0.98f, 0.98f));
            shader.setVec3("material.specular", glm::vec3(0.2f, 0.2f, 0.2f));
            shader.setFloat("material.shininess", 16.0f);
            
            model = glm::mat4(1.0f);
            float whiteCenterY = screenTopY - topFrameHeight - whiteHeight / 2.0f;
            model = glm::translate(model, glm::vec3(screenCenterX, whiteCenterY, screenZ + 0.01f));
            model = glm::scale(model, glm::vec3(whiteWidth, whiteHeight, screenThickness));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 12);
        }
        
        // 3. Draw bottom rod (weight bar) - as a solid cylinder
        shader.setVec3("material.ambient", glm::vec3(0.15f, 0.15f, 0.15f));
        shader.setVec3("material.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setFloat("material.shininess", 64.0f);
        
        float rodRadius = 0.025f;
        float rodLength = totalScreenWidth * 1.02f;
        float rodY = screenTopY - currentTotalScreenHeight - rodRadius + 0.003f;
        
        // Generate cylinder vertices for the rod
        std::vector<float> cylinderVerts = generateCylinderVertices(1.0f, 1.0f, 32); // 32 segments for smoothness
        
        unsigned int rodVAO, rodVBO;
        glGenVertexArrays(1, &rodVAO);
        glGenBuffers(1, &rodVBO);
        
        glBindVertexArray(rodVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rodVBO);
        glBufferData(GL_ARRAY_BUFFER, cylinderVerts.size() * sizeof(float), cylinderVerts.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(screenCenterX, rodY, screenZ + 0.02f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate to horizontal
        model = glm::scale(model, glm::vec3(rodRadius, rodLength, rodRadius));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, cylinderVerts.size() / 8);
        
        glDeleteVertexArrays(1, &rodVAO);
        glDeleteBuffers(1, &rodVBO);
        
        glDeleteVertexArrays(1, &screenVAO);
        glDeleteBuffers(1, &screenVBO);
    }
    
    // Reset model matrix
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}

unsigned int Classroom::loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded successfully: " << path << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
