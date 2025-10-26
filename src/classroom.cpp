#include "../include/classroom.h"
#include <iostream>

Classroom::Classroom()
{
    // Constructor - buffers will be initialized in initializeGeometry()
    fanRotation = 0.0f;
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
    glDeleteVertexArrays(1, &tablesVAO);
    glDeleteBuffers(1, &tablesVBO);
    glDeleteVertexArrays(1, &podiumVAO);
    glDeleteBuffers(1, &podiumVBO);
    glDeleteVertexArrays(1, &boardVAO);
    glDeleteBuffers(1, &boardVBO);
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
    generateTables();
    generatePodium();
    generateGreenBoard();
    generateLights();

    // Setup all buffers
    setupBuffers(floorVAO, floorVBO, floorVertices);
    setupBuffers(ceilingVAO, ceilingVBO, ceilingVertices);
    setupBuffers(wallsVAO, wallsVBO, wallVertices);
    setupBuffers(doorsVAO, doorsVBO, doorVertices);
    setupBuffers(windowsVAO, windowsVBO, windowVertices);
    setupBuffers(benchesVAO, benchesVBO, benchVertices);
    setupBuffers(tablesVAO, tablesVBO, tableVertices);
    setupBuffers(podiumVAO, podiumVBO, podiumVertices);
    setupBuffers(boardVAO, boardVBO, boardVertices);
    setupBuffers(lightsVAO, lightsVBO, lightVertices);
    
    // Load OBJ models
    if (!fanModel.loadOBJ("models/fan_up.obj"))
    {
        std::cout << "Warning: Failed to load fan model. Please place fan_up.obj in models/ directory" << std::endl;
    }
    
    // Disabled bench/table OBJ loading for better performance
    // Use procedural geometry instead (faster rendering)
    /*
    if (!benchModel.loadOBJ("models/classroom_desk.obj"))
    {
        std::cout << "Warning: Failed to load bench model. Please place bench.obj in models/ directory" << std::endl;
    }
    
    if (!tableModel.loadOBJ("models/table.obj"))
    {
        std::cout << "Warning: Failed to load table model. Please place table.obj in models/ directory" << std::endl;
    }
    */
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
    
    // Create ceiling as a large quad
    glm::vec3 v1(-ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    glm::vec3 v2(ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    glm::vec3 v3(ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    glm::vec3 v4(-ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    
    glm::vec3 normal(0.0f, -1.0f, 0.0f);
    
    addQuad(ceilingVertices, v1, v2, v3, v4, normal,
           glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 0.0f), 
           glm::vec2(4.0f, 3.0f), glm::vec2(0.0f, 3.0f));
}

void Classroom::generateWalls()
{
    wallVertices.clear();
    
    // Front wall (where green board is)
    glm::vec3 front_v1(-ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 front_v2(ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 front_v3(ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    glm::vec3 front_v4(-ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    
    addQuad(wallVertices, front_v1, front_v2, front_v3, front_v4, 
           glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(6.0f, 0.0f), 
           glm::vec2(6.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Back wall (entrance side)
    glm::vec3 back_v1(ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 back_v2(-ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 back_v3(-ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    glm::vec3 back_v4(ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    
    addQuad(wallVertices, back_v1, back_v2, back_v3, back_v4, 
           glm::vec3(0.0f, 0.0f, 1.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(6.0f, 0.0f), 
           glm::vec2(6.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Left wall (windows side)
    glm::vec3 left_v1(-ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 left_v2(-ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 left_v3(-ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    glm::vec3 left_v4(-ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    
    addQuad(wallVertices, left_v1, left_v2, left_v3, left_v4, 
           glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 0.0f), 
           glm::vec2(4.0f, 2.0f), glm::vec2(0.0f, 2.0f));
    
    // Right wall
    glm::vec3 right_v1(ROOM_WIDTH/2, 0.0f, ROOM_LENGTH/2);
    glm::vec3 right_v2(ROOM_WIDTH/2, 0.0f, -ROOM_LENGTH/2);
    glm::vec3 right_v3(ROOM_WIDTH/2, ROOM_HEIGHT, -ROOM_LENGTH/2);
    glm::vec3 right_v4(ROOM_WIDTH/2, ROOM_HEIGHT, ROOM_LENGTH/2);
    
    addQuad(wallVertices, right_v1, right_v2, right_v3, right_v4, 
           glm::vec3(-1.0f, 0.0f, 0.0f),
           glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 0.0f), 
           glm::vec2(4.0f, 2.0f), glm::vec2(0.0f, 2.0f));
}

void Classroom::generateDoors()
{
    doorVertices.clear();
    
    // Door on the left wall near the front
    float doorWidth = 1.0f;
    float doorHeight = 2.1f;
    float doorZ = -ROOM_LENGTH/2 + 1.5f; // Near the front wall (1.5m from front)
    
    addCube(doorVertices, 
           glm::vec3(-ROOM_WIDTH/2 + 0.05f, doorHeight/2, doorZ), 
           glm::vec3(0.001f, doorHeight, doorWidth));
}

void Classroom::generateWindows()
{
    windowVertices.clear();
    // No windows - keep the function but don't generate any geometry
}

void Classroom::generateBenches()
{
    benchVertices.clear();
    
    // Realistic classroom bench dimensions
    float benchWidth = 1.8f;
    float benchDepth = 0.45f;     // Slightly deeper for comfort
    float seatHeight = 0.45f;     // Standard sitting height
    float seatThickness = 0.05f;  // Realistic seat thickness
    float backrestHeight = 0.35f; // Backrest height above seat
    float backrestThickness = 0.04f;
    float legWidth = 0.06f;
    float legDepth = 0.06f;
    
    // Support beam dimensions
    float beamWidth = benchWidth;
    float beamHeight = 0.08f;
    float beamDepth = 0.06f;
    
    // 4 rows of benches, 3 benches per row - shifted forward from green board
    for(int row = 0; row < 4; row++)
    {
        for(int col = 0; col < 3; col++)
        {
            float x = -3.0f + col * 2.2f;
            float z = -0.5f + row * 1.2f; // Shifted 1.5m forward to create gap from green board
            
            // Bench seat (main seating surface)
            addCube(benchVertices, 
                   glm::vec3(x, seatHeight, z), 
                   glm::vec3(benchWidth, seatThickness, benchDepth));
            
            // Backrest
            addCube(benchVertices, 
                   glm::vec3(x, seatHeight + seatThickness/2 + backrestHeight/2, z + benchDepth/2 - backrestThickness/2), 
                   glm::vec3(benchWidth, backrestHeight, backrestThickness));
            
            // Front legs (2 legs)
            float frontLegZ = z - benchDepth/2 + legDepth/2;
            addCube(benchVertices, 
                   glm::vec3(x - benchWidth/2 + legWidth/2, seatHeight/2, frontLegZ), 
                   glm::vec3(legWidth, seatHeight, legDepth));
            addCube(benchVertices, 
                   glm::vec3(x + benchWidth/2 - legWidth/2, seatHeight/2, frontLegZ), 
                   glm::vec3(legWidth, seatHeight, legDepth));
            
            // Back legs (2 legs) - supporting the backrest
            float backLegZ = z + benchDepth/2 - legDepth/2;
            float backLegHeight = seatHeight + backrestHeight;
            addCube(benchVertices, 
                   glm::vec3(x - benchWidth/2 + legWidth/2, backLegHeight/2, backLegZ), 
                   glm::vec3(legWidth, backLegHeight, legDepth));
            addCube(benchVertices, 
                   glm::vec3(x + benchWidth/2 - legWidth/2, backLegHeight/2, backLegZ), 
                   glm::vec3(legWidth, backLegHeight, legDepth));
            
            // Horizontal support beams for stability
            // Front support beam
            addCube(benchVertices, 
                   glm::vec3(x, seatHeight * 0.3f, frontLegZ), 
                   glm::vec3(beamWidth, beamHeight, beamDepth));
            
            // Back support beam
            addCube(benchVertices, 
                   glm::vec3(x, seatHeight * 0.3f, backLegZ), 
                   glm::vec3(beamWidth, beamHeight, beamDepth));
            
            // Side support beams (connecting front and back)
            float sideBeamX1 = x - benchWidth/2 + legWidth/2;
            float sideBeamX2 = x + benchWidth/2 - legWidth/2;
            addCube(benchVertices, 
                   glm::vec3(sideBeamX1, seatHeight * 0.3f, z), 
                   glm::vec3(beamDepth, beamHeight, benchDepth - legDepth));
            addCube(benchVertices, 
                   glm::vec3(sideBeamX2, seatHeight * 0.3f, z), 
                   glm::vec3(beamDepth, beamHeight, benchDepth - legDepth));
        }
    }
}

void Classroom::generateTables()
{
    tableVertices.clear();
    
    // Tables positioned in front of benches
    float tableWidth = 1.8f;
    float tableDepth = 0.6f;
    float tableHeight = 0.75f;
    float tableTopThickness = 0.08f; // Make table top thicker for visibility
    
    for(int row = 0; row < 4; row++)
    {
        for(int col = 0; col < 3; col++)
        {
            float x = -3.0f + col * 2.2f;
            float z = -0.5f + row * 1.2f - 0.6f; // In front of benches (shifted forward with benches)
            
            // Table top
            addCube(tableVertices, 
                   glm::vec3(x, tableHeight - tableTopThickness/2, z), 
                   glm::vec3(tableWidth, tableTopThickness, tableDepth));
            
            // Table legs
            float legWidth = 0.08f; // Make legs thicker for visibility
            float legHeight = tableHeight - tableTopThickness;
            
            for(int leg = 0; leg < 4; leg++)
            {
                float legX = x + (leg % 2 == 0 ? -tableWidth/2 + legWidth/2 : tableWidth/2 - legWidth/2);
                float legZ = z + (leg < 2 ? -tableDepth/2 + legWidth/2 : tableDepth/2 - legWidth/2);
                
                addCube(tableVertices, 
                       glm::vec3(legX, legHeight/2, legZ), 
                       glm::vec3(legWidth, legHeight, legWidth));
            }
        }
    }
}

void Classroom::generatePodium()
{
    podiumVertices.clear();
    
    // Teacher's podium beside the green board on back wall
    float podiumWidth = 0.8f;  // Decreased width for better proportions
    float podiumDepth = 0.8f;
    float podiumHeight = 1.2f; // Increased height for better functionality
    
    // Position podium to the right side of the green board - shifted for better accessibility
    float podiumX = 3.2f; // Slightly closer to center for better positioning relative to benches
    float podiumZ = -ROOM_LENGTH/2 + podiumDepth/2 + 0.8f; // Further from back wall, closer to teaching area
    
    addCube(podiumVertices, 
           glm::vec3(podiumX, podiumHeight/2, podiumZ), 
           glm::vec3(podiumWidth, podiumHeight, podiumDepth));
}

void Classroom::generateGreenBoard()
{
    boardVertices.clear();
    
    // Green board on the back wall - larger and more prominent
    float boardWidth = 5.0f;  // Wider board
    float boardHeight = 1.5f; // Taller board
    float boardY = 1.8f;      // Slightly lower for better visibility
    float boardThickness = 0.08f; // Thicker for better visibility
    
    addCube(boardVertices, 
           glm::vec3(0.0f, boardY, -ROOM_LENGTH/2 + boardThickness/2), 
           glm::vec3(boardWidth, boardHeight, boardThickness));
}

void Classroom::generateLights()
{
    lightVertices.clear();
    
    // Ceiling lights - fluorescent tubes
    float lightWidth = 1.5f;
    float lightDepth = 0.3f;
    float lightHeight = 0.1f;
    
    // 6 lights arranged in 2 rows
    for(int row = 0; row < 2; row++)
    {
        for(int col = 0; col < 3; col++)
        {
            float x = -3.0f + col * 3.0f;
            float z = -1.0f + row * 2.0f;
            
            addCube(lightVertices, 
                   glm::vec3(x, ROOM_HEIGHT - lightHeight/2, z), 
                   glm::vec3(lightWidth, lightHeight, lightDepth));
        }
    }
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
    // Render floor (gray tiles)
    renderBuffer(floorVAO, floorVertices.size() / 8, 
                glm::vec3(0.4f, 0.4f, 0.4f), 
                glm::vec3(0.6f, 0.6f, 0.6f), 
                glm::vec3(0.2f, 0.2f, 0.2f), shader);
    
    // Render ceiling (white)
    renderBuffer(ceilingVAO, ceilingVertices.size() / 8, 
                glm::vec3(0.8f, 0.8f, 0.8f), 
                glm::vec3(0.9f, 0.9f, 0.9f), 
                glm::vec3(0.1f, 0.1f, 0.1f), shader);
    
    // Render walls (light beige)
    renderBuffer(wallsVAO, wallVertices.size() / 8, 
                glm::vec3(0.8f, 0.75f, 0.65f), 
                glm::vec3(0.9f, 0.85f, 0.75f), 
                glm::vec3(0.1f, 0.1f, 0.1f), shader);
    

       // Render door on right wall (realistic brown wood)
       renderBuffer(doorsVAO, doorVertices.size() / 8, 
       glm::vec3(0.25f, 0.13f, 0.05f),   // ambient - darker base tone
       glm::vec3(0.55f, 0.27f, 0.07f),   // diffuse - natural brown
       glm::vec3(0.2f, 0.15f, 0.1f),     // specular - soft reflection
       shader);
    
    // No windows or podium in this classroom design

       // Render benches (realistic polished wood)
       renderBuffer(benchesVAO, benchVertices.size() / 8,
       glm::vec3(0.35f, 0.20f, 0.07f),   // ambient - darker base
       glm::vec3(0.65f, 0.40f, 0.15f),   // diffuse - rich teak color
       glm::vec3(0.25f, 0.18f, 0.10f),   // specular - slight shine
       shader);

       // Render tables (realistic dark wood)
       renderBuffer(tablesVAO, tableVertices.size() / 8,
       glm::vec3(0.20f, 0.10f, 0.05f),   // ambient - dark base
       glm::vec3(0.45f, 0.25f, 0.10f),   // diffuse - deep brown
       glm::vec3(0.15f, 0.10f, 0.05f),   // specular - subtle shine
       shader);


    
    // Render podium (dark wood beside green board)
    renderBuffer(podiumVAO, podiumVertices.size() / 8, 
                glm::vec3(0.2f, 0.15f, 0.1f), 
                glm::vec3(0.4f, 0.3f, 0.2f), 
                glm::vec3(0.15f, 0.1f, 0.08f), shader);
    
    // Render green board (main focal point of classroom)
    renderBuffer(boardVAO, boardVertices.size() / 8,
    glm::vec3(0.0f, 0.2f, 0.0f),   // ambient - very dark
    glm::vec3(0.0f, 0.4f, 0.0f),   // diffuse - dark green
    glm::vec3(0.05f, 0.1f, 0.05f), // specular - subtle highlights
    shader);
}

void Classroom::renderLights(Shader& lightShader)
{
    // Set light color
    lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.9f));
    
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

void Classroom::renderFan(Shader& shader)
{
    if (fanModel.vertices.empty())
        return;  // Fan model not loaded
    
    // Set fan material properties (bright cream/white color for visibility)
    shader.setVec3("material.ambient", glm::vec3(0.6f, 0.55f, 0.5f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.85f, 0.75f));
    shader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    shader.setFloat("material.shininess", 32.0f);
    
    // Render LEFT fan
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, ROOM_HEIGHT - 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Render RIGHT fan
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, ROOM_HEIGHT - 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setMat4("model", model);
    fanModel.render();
    
    // Reset model matrix
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
}
