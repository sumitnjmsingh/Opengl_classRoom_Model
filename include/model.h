#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Model
{
public:
    std::vector<float> vertices;  // Interleaved: position (3) + normal (3) + texcoord (2)
    unsigned int VAO, VBO;
    
    Model() : VAO(0), VBO(0) {}
    
    ~Model()
    {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
    }
    
    // Load OBJ file and setup buffers
    bool loadOBJ(const std::string& path)
    {
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_uvs;
        
        std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
        
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cout << "ERROR::MODEL::Failed to open OBJ file: " << path << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v")  // Vertex position
            {
                glm::vec3 vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            }
            else if (prefix == "vn")  // Vertex normal
            {
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "vt")  // Texture coordinate
            {
                glm::vec2 uv;
                iss >> uv.x >> uv.y;
                temp_uvs.push_back(uv);
            }
            else if (prefix == "f")  // Face
            {
                std::string vertex1, vertex2, vertex3;
                iss >> vertex1 >> vertex2 >> vertex3;
                
                // Parse face indices (format: v/vt/vn or v//vn or v/vt or v)
                auto parseVertex = [&](const std::string& vertexStr) {
                    unsigned int vIdx = 0, vtIdx = 0, vnIdx = 0;
                    
                    size_t firstSlash = vertexStr.find('/');
                    if (firstSlash == std::string::npos)
                    {
                        // Format: v
                        vIdx = std::stoi(vertexStr);
                    }
                    else
                    {
                        vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                        
                        size_t secondSlash = vertexStr.find('/', firstSlash + 1);
                        if (secondSlash == std::string::npos)
                        {
                            // Format: v/vt
                            vtIdx = std::stoi(vertexStr.substr(firstSlash + 1));
                        }
                        else
                        {
                            if (secondSlash - firstSlash > 1)
                            {
                                // Format: v/vt/vn
                                vtIdx = std::stoi(vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                            }
                            // Format: v//vn or v/vt/vn
                            vnIdx = std::stoi(vertexStr.substr(secondSlash + 1));
                        }
                    }
                    
                    vertexIndices.push_back(vIdx);
                    if (vtIdx > 0) uvIndices.push_back(vtIdx);
                    if (vnIdx > 0) normalIndices.push_back(vnIdx);
                };
                
                parseVertex(vertex1);
                parseVertex(vertex2);
                parseVertex(vertex3);
            }
        }
        
        file.close();
        
        // Build interleaved vertex data
        vertices.clear();
        bool hasNormals = normalIndices.size() == vertexIndices.size();
        bool hasUVs = uvIndices.size() == vertexIndices.size();
        
        for (size_t i = 0; i < vertexIndices.size(); i++)
        {
            // Position
            glm::vec3 pos = temp_vertices[vertexIndices[i] - 1];
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
            
            // Normal
            if (hasNormals)
            {
                glm::vec3 normal = temp_normals[normalIndices[i] - 1];
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
            }
            else
            {
                // Default normal
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
            
            // UV
            if (hasUVs)
            {
                glm::vec2 uv = temp_uvs[uvIndices[i] - 1];
                vertices.push_back(uv.x);
                vertices.push_back(uv.y);
            }
            else
            {
                // Default UV
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
        
        std::cout << "MODEL::Loaded OBJ file: " << path << std::endl;
        std::cout << "  Vertices: " << temp_vertices.size() << std::endl;
        std::cout << "  Normals: " << temp_normals.size() << std::endl;
        std::cout << "  Faces: " << vertexIndices.size() / 3 << std::endl;
        
        setupBuffers();
        return true;
    }
    
    void setupBuffers()
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
    
    void render()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
        glBindVertexArray(0);
    }
};

#endif
