//
// Created by Gamer on 23/06/2025.
//

#include <vector>
#include "../Block/Block.h"

using namespace std;
using namespace glm;

#ifndef CHUNK_H
#define CHUNK_H

struct Chunk_Mesh {
	vector<float> vertices;
	vector<unsigned int> indices;
};

class Chunk {
	
	public:
		vector<Block> blocks;
		int block_count;
		vec3 world_position = vec3(0, 0, 0);
		bool empty = false;
		bool generated = false;
		bool has_mesh = false;
		Chunk_Mesh Stored_mesh;

		void Generate();
		Chunk_Mesh Assemble_Mesh();

};



#endif //CHUNK_H
