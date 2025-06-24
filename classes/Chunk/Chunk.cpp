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
		int y = (i / 32) % 32;
		int z = i / (32 * 32);

		newBlock.chunk_position = vec3(x, y, z);
		newBlock.world_position = newBlock.chunk_position + world_position;

		blocks.push_back(newBlock);
	}

	empty = false;
	generated = true;
};

Chunk_Mesh Chunk::Assemble_Mesh() {
	Chunk_Mesh mesh;
	std::cout << blocks.size() << std::endl;

	unsigned int vertex_offset = 0;

	for (int i = 0; i < blocks.size(); i++) {
		Block& block = blocks[i];

		std::vector<float> blockvertices = block.assemble_vertices({0, 1, 2, 3, 4, 5});
		std::vector<unsigned int> blockindices = block.assemble_indices({0, 1, 2, 3, 4, 5});

		// Append all floats (pos, normal, uv) directly
		mesh.vertices.insert(mesh.vertices.end(), blockvertices.begin(), blockvertices.end());


		// Update indices with offset and append
		for (unsigned int index : blockindices) {
			mesh.indices.push_back(index + vertex_offset);
		}

		// Each face = 4 vertices → 6 faces per block = 24 vertices
		// Each vertex has 8 floats (x, y, z, nx, ny, nz, u, v)
		// So offset increases by 24 for each block
		vertex_offset += blockvertices.size() / 8;
	}

	Stored_mesh = mesh;
	has_mesh = true;
	return mesh;
};