#ifndef CHUNK_H
#define CHUNK_H

#pragma once

const int chunkSize = 16;

struct Chunk
{
	int x, z;
	int blocksHeight[chunkSize][chunkSize];
	int blocksType[chunkSize][chunkSize];

	Chunk(int x, int z) { this->x = x; this->z = z; }
};

#endif // !CHUNK_H
