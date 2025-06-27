//
// Created by Gamer on 23/06/2025.
//

#include "Chunk.h"

#include <iostream>
#include <bits/ostream.tcc>


void Chunk::Generate() {

	blocks.reserve(32 * 32 * 32); // Optional: avoid reallocations

	for (int i = 0;i < 32*32*32;i++) {
		Block newBlock = Block();

		int x = i % 32;
		int y = (i / 32) % 32 ;
		int z = i / (32 * 32);

		newBlock.chunk_position = vec3(x, y, z);
		newBlock.world_position = newBlock.chunk_position + world_position;
		if (y < 25)
			newBlock.type == BlockType::Stone;
		else if (y < 31)
			newBlock.type = BlockType::Dirt;
		else
			newBlock.type = BlockType::Grass;
		blocks.push_back(newBlock);
	}

	empty = false;
	generated = true;
};

bool Chunk::GetBlockAt(glm::ivec3 pos) const {
	if (pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= 32 || pos.y >= 32 || pos.z >= 32)
		return false;

	int index = pos.y * 32 * 32 + pos.z * 32 + pos.x;
	return blocks[index].solid; // Or any check for active/visible block
}

Chunk_Mesh Chunk::Assemble_Mesh() {
	Chunk_Mesh& mesh = Stored_mesh;
	unsigned int vertex_offset = 0;

	for (int i = 0; i < blocks.size(); ++i) {
		Block& block = blocks[i];
		glm::ivec3 pos = glm::ivec3(block.chunk_position);

		std::vector<int> visible_faces;

		// Check 6 directions for adjacent blocks (naive method)
		if (!GetBlockAt(pos + glm::ivec3(0, 0, -1))) visible_faces.push_back(0); // back
		if (!GetBlockAt(pos + glm::ivec3(0, 0,  1))) visible_faces.push_back(1); // front
		if (!GetBlockAt(pos + glm::ivec3(-1, 0, 0))) visible_faces.push_back(2); // left
		if (!GetBlockAt(pos + glm::ivec3(1, 0,  0))) visible_faces.push_back(3); // right
		if (!GetBlockAt(pos + glm::ivec3(0, -1, 0))) visible_faces.push_back(4); // bottom
		if (!GetBlockAt(pos + glm::ivec3(0,  1, 0))) visible_faces.push_back(5); // top

		if (visible_faces.empty()) continue; // fully occluded block

		std::vector<float> blockvertices = block.assemble_vertices(visible_faces);
		std::vector<unsigned int> blockindices = block.assemble_indices(visible_faces);

		mesh.vertices.insert(mesh.vertices.end(), blockvertices.begin(), blockvertices.end());

		for (unsigned int index : blockindices) {
			mesh.indices.push_back(index + vertex_offset);
		}

		// Each face = 4 vertices → offset is face count * 4
		vertex_offset += (blockvertices.size() / 8);
	}

	has_mesh = true;
	return Stored_mesh;
}
