//
// Created by Gamer on 22/06/2025.
//

#include <vector>
#include <glm/glm.hpp>

#ifndef BLOCK_H
#define BLOCK_H
#include "glm/vec3.hpp"
#include "glm/detail/type_vec1.hpp"
#include <unordered_map>

enum class BlockType {
 Stone,
 Dirt,
 Grass,
};

struct BlockInfo {
 const char* name;
 float durability;
 bool usespecular;
 glm::vec2 Atlas_coordinate;
 glm::vec3 direction = glm::vec3(0.0f);
};

const std::unordered_map<BlockType, BlockInfo> BlockDatabase = {
 { BlockType::Stone, { "Stone", 3.0f,false, glm::vec2(1.f,0.f)}},
    {BlockType::Dirt,{"Dirt",0.4f,false,glm::vec2(2.f,0.f)}},
 {BlockType::Grass,{"Grass",0.5f,false,glm::vec2(3.f,0.f),glm::vec3(0,1,0)}}
};

inline std::vector<float> base_faces = {
 // Back face
 -0.5f, -0.5f, -0.5f,   0.f, 0.f, -1.f,  0.f, 0.f,
  0.5f, -0.5f, -0.5f,   0.f, 0.f, -1.f,  1.f, 0.f,
  0.5f,  0.5f, -0.5f,   0.f, 0.f, -1.f,  1.f, 1.f,
 -0.5f,  0.5f, -0.5f,   0.f, 0.f, -1.f,  0.f, 1.f,

 // Front face
 -0.5f, -0.5f,  0.5f,   0.f, 0.f, 1.f,   0.f, 0.f,
  0.5f, -0.5f,  0.5f,   0.f, 0.f, 1.f,   1.f, 0.f,
  0.5f,  0.5f,  0.5f,   0.f, 0.f, 1.f,   1.f, 1.f,
 -0.5f,  0.5f,  0.5f,   0.f, 0.f, 1.f,   0.f, 1.f,

 // Left face
 -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,   0.f, 0.f,
 -0.5f,  0.5f, -0.5f,  -1.f, 0.f, 0.f,   1.f, 0.f,
 -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,   1.f, 1.f,
 -0.5f, -0.5f,  0.5f,  -1.f, 0.f, 0.f,   0.f, 1.f,

 // Right face
  0.5f, -0.5f, -0.5f,   1.f, 0.f, 0.f,   0.f, 0.f,
  0.5f,  0.5f, -0.5f,   1.f, 0.f, 0.f,   1.f, 0.f,
  0.5f,  0.5f,  0.5f,   1.f, 0.f, 0.f,   1.f, 1.f,
  0.5f, -0.5f,  0.5f,   1.f, 0.f, 0.f,   0.f, 1.f,

 // Bottom face
 -0.5f, -0.5f, -0.5f,   0.f, -1.f, 0.f,  0.f, 1.f,
  0.5f, -0.5f, -0.5f,   0.f, -1.f, 0.f,  1.f, 1.f,
  0.5f, -0.5f,  0.5f,   0.f, -1.f, 0.f,  1.f, 0.f,
 -0.5f, -0.5f,  0.5f,   0.f, -1.f, 0.f,  0.f, 0.f,

 // Top face
 -0.5f,  0.5f, -0.5f,   0.f, 1.f, 0.f,   0.f, 1.f,
  0.5f,  0.5f, -0.5f,   0.f, 1.f, 0.f,   1.f, 1.f,
  0.5f,  0.5f,  0.5f,   0.f, 1.f, 0.f,   1.f, 0.f,
 -0.5f,  0.5f,  0.5f,   0.f, 1.f, 0.f,   0.f, 0.f,
};

inline std::vector<unsigned int> base_indices = {
 // Back
 0, 1, 2,  2, 3, 0,
 // Front
 4, 5, 6,  6, 7, 4,
 // Left
 8, 9,10, 10,11, 8,
 // Right
12,13,14, 14,15,12,
 // Bottom
16,17,18, 18,19,16,
 // Top
20,21,22, 22,23,20
};


class Block {
public:
 glm::vec3 chunk_position;
 glm::vec3 world_position;
 BlockType type;
 bool solid = true;
  std::vector<float> assemble_vertices(std::vector<int> visible_faces);
  std::vector<unsigned int> assemble_indices(std::vector<int> visible_faces);


};



#endif //BLOCK_H
