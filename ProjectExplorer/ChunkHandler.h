#ifndef CHUNK_HANDLER_H
#define CHUNK_HANDLER_H

#pragma once

#include <iostream>
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
	std::vector<Chunk> chunks;
	std::vector<int> visibleChunks;

	GLint numberOfVisibleCubes = 0;
	glm::mat4 * translations;

	ChunkHandler();
	ChunkHandler(float playerXPos, float playerZPos);
	ChunkHandler(float playerXPos, float playerZPos, unsigned int seed);
	~ChunkHandler();

	void updateVisibleChunks(float playerXPos, float playerZPos);
	void updatePlayerPosition(float playerXPos, float playerZPos);
	int getHeightValue(float playerXPos, float playerZPos);
	float getVisibleChunkSide();
	void setVisibleChunkSid(int newVisibleChunkSide);
private:
	// Stores the position of the chunk and the index of the chunk in the chunk vector
	std::map<std::pair<int, int>, int> worldmap;
	std::pair<int, int> currentChunkIndex;

	PerlinNoise perlin;

	// Seed to generate terrain. Can only be set in the constructor
	unsigned int seed;

	// Random variables used in the terrain generation
	int randomMin = 0;
	int randomMax = 100;

	// Random variables used in the terrain generation
	int playerLastX = 0;
	int playerLastZ = 0;

	// This is how many chunk will be generated around the player
	int initialChunkSide = 7;

	// This is how many chunk will be visible by the player
	int visibleChunkSide = 11;

	void generateChunk(int chunkX, int chunkZ);
	void generateInitialChunks(int chunkX, int chunkZ);
	void populateTranslationsArray();
};

#endif // !CHUNK_HANDLER_H
