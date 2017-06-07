#ifndef CHUNK_HANDLER_H
#define CHUNK_HANDLER_H

#pragma once

#include <vector>
#include <map>

#include "Chunk.h"
#include "PerlinNoise.h"

struct Vector2
{
	int x;
	int y;

	Vector2(int x, int y) { this->x = x; this->y = y; }
};

struct Vector2Compare
{
	bool operator() (const Vector2& lhs, const Vector2& rhs) const
	{
		return lhs.x < rhs.x && lhs.y < rhs.y;
	}
};


class ChunkHandler
{
public:
	ChunkHandler(float playerXPos, float playerZPos);
	ChunkHandler(float playerXPos, float playerZPos, unsigned int seed);
	~ChunkHandler();

	void updateVisibleChunks(float playerXPos, float playerZPos);
private:
	// Stores the position of the chunk and the index of the chunk in the chunk vector
	std::map<std::pair<int, int>, int> worldmap;
	std::vector<Chunk> chunks;
	std::vector<int> visibleChunks;

	PerlinNoise perlin;

	// Seed to generate terrain. Can only be set in the constructor
	unsigned int seed;

	// Random variables used in the terrain generation
	const int randomMin = 0;
	const int randomMax = 100;

	// This is how many chunk will be generated around the player
	const int initialChunkSide = 5;

	// This is how many chunk will be visible by the player
	const int visibleChunkSide = 3;
};

#endif // !CHUNK_HANDLER_H
