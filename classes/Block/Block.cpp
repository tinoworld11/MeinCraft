//
// Created by Gamer on 22/06/2025.
//

#include "Block.h"

#include <algorithm>
#include <valarray>


std::vector<float> Block::assemble_vertices(std::vector<unsigned int> visible_faces) {
	std::vector<float> faces = base_faces;

	for (size_t i = 0; i < faces.size(); i += 8) {
		faces[i + 0] += chunk_position.x; // x
		faces[i + 1] += chunk_position.y; // y
		faces[i + 2] += chunk_position.z; // z
	}

	return faces;
}


std::vector<unsigned int> Block::assemble_indices(std::vector<unsigned int> visible_faces) {
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