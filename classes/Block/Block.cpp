//
// Created by Gamer on 22/06/2025.
//

#include "Block.h"

#include <algorithm>
#include <valarray>


std::vector<float> Block::assemble_vertices(std::vector<int> visible_faces) {
	std::vector<float> faces = base_faces;

	const float atlas_size = 16.0f;
	const float tile_size = 1.0f / atlas_size;
	const float padding = 0.004f;

	// Stone texture at (1, 0)
	glm::vec2 uv_pos = BlockDatabase.at(type).Atlas_coordinate;
	const int tile_u = uv_pos.x;
	const int tile_v = 15-uv_pos.y;

	float min_u = tile_u * tile_size + padding;
	float max_u = (tile_u + 1) * tile_size - padding;
	float min_v = tile_v * tile_size + padding;
	float max_v = (tile_v + 1) * tile_size - padding;

	for (size_t i = 0; i < faces.size(); i += 8) {
		// Add block position offset
		faces[i + 0] += chunk_position.x;
		faces[i + 1] += chunk_position.y;
		faces[i + 2] += chunk_position.z;

		// Remap UVs into selected tile with padding
		float original_u = faces[i + 6]; // expected in range [0.0, 1.0]
		float original_v = faces[i + 7];

		faces[i + 6] = min_u + original_u * (max_u - min_u);
		faces[i + 7] = min_v + original_v * (max_v - min_v);
	}

	return faces;
}


std::vector<unsigned int> Block::assemble_indices(std::vector< int> visible_faces) {
	std::vector<unsigned int> indices;

	for (int i = 0; i < 6; i++) {
		if (std::count(visible_faces.begin(), visible_faces.end(), i) > 0) {
			//0 = back 1 front 2 left 3 right 4 down 5 up

			std::vector<unsigned int> tmp;
			indices.insert(indices.end(), base_indices.begin() + i * 6, base_indices.begin() + i * 6 + 6);
		}
	}
	return  indices;
}