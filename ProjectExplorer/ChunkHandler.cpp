#include "ChunkHandler.h"
#include "BMath.h"

ChunkHandler::ChunkHandler(float playerXPos, float playerZPos)
{
	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkX = std::floor(xPos / chunkSize);
	int chunkZ = std::floor(zPos / chunkSize);

	// Calculate the position of the farthest chunks that will be initially generated
	int firstChunkX = chunkX - (this->initialChunkSide * chunkSize);
	int  lastChunkX = chunkX + (this->initialChunkSide * chunkSize);
	int firstChunkZ = chunkZ - (this->initialChunkSide * chunkSize);
	int  lastChunkZ = chunkZ + (this->initialChunkSide * chunkSize);

	for (int i = firstChunkX; i < lastChunkX; i += chunkSize)
	{
		for (int j = firstChunkZ; j < lastChunkZ; j += chunkSize)
		{
			int chunkXIndex = i / 16;
			int chunkZIndex = j / 16;

			//worldmap.insert(std::pair<std::pair<int, int>, int>(std::pair<int, int>(chunkXIndex, chunkZIndex), chunks.size()));
			worldmap[std::pair<int, int>(chunkXIndex, chunkZIndex)] = chunks.size();
			chunks.push_back(Chunk(i, j));

			// At this point I know which chunk to generate
			// Now I enter the chunk and generate all its blocks
			for (int x = 0; x < chunkSize; x++)
			{
				for (int z = 0; z < chunkSize; z++)
				{
					// Make values for terrain generation positive
					int positiveI = (i < 0) ? (i * -1) - x : i + x;
					int positiveJ = (j < 0) ? (j * -1) - z : j + z;

					//Populate chunk
					int randNum = this->randomMin + (rand() % (int)(this->randomMax - this->randomMin + 1));

					// Normalize absolute x and z positions to 0-1 range
					GLfloat perlinX = bmath::norm(positiveI, 0, 255);
					GLfloat perlinZ = bmath::norm(positiveJ, 0, 255);

					GLfloat perlinValue = this->perlin.noise(perlinX, perlinZ, 0.0f);

					// Map absolute x and z positions back to 0-255 range
					GLfloat mappedValue = std::floor(bmath::map(perlinValue, 0.0f, 1.0f, 0.0f, 255.0f));

					chunks.back().blocks[x][z] = mappedValue;
				}
			}
		}
	}
}

ChunkHandler::ChunkHandler(float playerXPos, float playerZPos, unsigned int seed)
{
	this->seed = seed;

	perlin = PerlinNoise(seed);

	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkX = std::floor(xPos / chunkSize);
	int chunkZ = std::floor(zPos / chunkSize);

	// Calculate the position of the farthest chunks that will be initially generated
	int firstChunkX = chunkX - (this->initialChunkSide * chunkSize);
	int  lastChunkX = chunkX + (this->initialChunkSide * chunkSize);
	int firstChunkZ = chunkZ - (this->initialChunkSide * chunkSize);
	int  lastChunkZ = chunkZ + (this->initialChunkSide * chunkSize);

	for (int i = firstChunkX; i < lastChunkX; i += chunkSize)
	{
		for (int j = firstChunkZ; j < lastChunkZ; j += chunkSize)
		{
			int chunkXIndex = i / 16;
			int chunkZIndex = j / 16;

			//worldmap.insert(std::pair<std::pair<int, int>, int>(std::pair<int, int>(chunkXIndex, chunkZIndex), chunks.size()));
			worldmap[std::pair<int, int>(chunkXIndex, chunkZIndex)] = chunks.size();
			chunks.push_back(Chunk(i, j));

			// At this point I know which chunk to generate
			// Now I enter the chunk and generate all its blocks
			for (int x = 0; x < chunkSize; x++)
			{
				for (int z = 0; z < chunkSize; z++)
				{
					// Make values for terrain generation positive
					int positiveI = (i < 0) ? (i * -1) - x : i + x;
					int positiveJ = (j < 0) ? (j * -1) - z : j + z;

					//Populate chunk
					int randNum = this->randomMin + (rand() % (int)(this->randomMax - this->randomMin + 1));

					// Normalize absolute x and z positions to 0-1 range
					GLfloat perlinX = bmath::norm(positiveI, 0, 255);
					GLfloat perlinZ = bmath::norm(positiveJ, 0, 255);

					GLfloat perlinValue = this->perlin.noise(perlinX, perlinZ, 0.0f);

					// Map absolute x and z positions back to 0-255 range
					GLfloat mappedValue = std::floor(bmath::map(perlinValue, 0.0f, 1.0f, 0.0f, 255.0f));

					chunks.back().blocks[x][z] = mappedValue;
				}
			}
		}
	}

	updateVisibleChunks(playerXPos, playerZPos);
}

ChunkHandler::~ChunkHandler()
{
}

void ChunkHandler::updateVisibleChunks(float playerXPos, float playerZPos)
{
	visibleChunks.clear();

	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkX = std::floor(xPos / chunkSize);
	int chunkZ = std::floor(zPos / chunkSize);


	for (int i = chunkX - 1; i < visibleChunkSide - 1; i++)
	{
		for (int j = chunkX - 1; j < visibleChunkSide - 1; j++)
		{
			int chunkIndex = worldmap[std::pair<int, int>(i, j)];
			visibleChunks.push_back(chunkIndex);
		}
	}

	int i = 0;
}
