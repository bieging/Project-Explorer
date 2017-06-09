#include "ChunkHandler.h"
#include "BMath.h"

ChunkHandler::ChunkHandler() {}

ChunkHandler::ChunkHandler(float playerXPos, float playerZPos)
{
	numberOfVisibleCubes = chunkSize * chunkSize * visibleChunkSide * visibleChunkSide;

	translations = new glm::mat4[numberOfVisibleCubes];

	playerLastX = playerXPos;
	playerLastZ = playerZPos;

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
					// Get absolute positions to use in the noise function
					int absoluteX = i + x;
					int absoluteY = j + z;

					// Normalize absolute x and z positions to 0-1 range
					GLfloat perlinX = bmath::norm(absoluteX, 0, 255);
					GLfloat perlinZ = bmath::norm(absoluteY, 0, 255);

					GLfloat perlinValue = this->perlin.noise(perlinX, perlinZ, 0.0f);

					// Map absolute x and z positions back to 0-255 range
					GLfloat mappedValue = std::floor(bmath::map(perlinValue, 0.0f, 1.0f, 0.0f, 255.0f));

					// Add height value to block
					chunks.back().blocksHeight[x][z] = mappedValue;
					// Add a random block type value
					int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));
					chunks.back().blocksType[x][z] = (randNum < 50) ? 0 : 1;
				}
			}
		}
	}
}

ChunkHandler::ChunkHandler(float playerXPos, float playerZPos, unsigned int seed)
{
	numberOfVisibleCubes = chunkSize * chunkSize * visibleChunkSide * visibleChunkSide;

	translations = new glm::mat4[numberOfVisibleCubes];

	playerLastX = playerXPos;
	playerLastZ = playerZPos;

	this->seed = seed;

	perlin = PerlinNoise(seed);

	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkX = std::floor(xPos / chunkSize) * chunkSize;
	int chunkZ = std::floor(zPos / chunkSize) * chunkSize;

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
					// Get absolute positions to use in the noise function
					int absoluteX = i + x;
					int absoluteY = j + z;

					// Normalize absolute x and z positions to 0-1 range
					GLfloat perlinX = bmath::norm(absoluteX, 0, 255);
					GLfloat perlinZ = bmath::norm(absoluteY, 0, 255);

					GLfloat perlinValue = this->perlin.noise(perlinX, perlinZ, 0.0f);

					// Map absolute x and z positions back to 0-255 range
					GLfloat mappedValue = std::floor(bmath::map(perlinValue, 0.0f, 1.0f, 0.0f, 255.0f));

					// Add height value to block
					chunks.back().blocksHeight[x][z] = mappedValue;
					// Add a random block type value
					int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));
					chunks.back().blocksType[x][z] = (randNum < 50) ? 0 : 1;
				}
			}
		}
	}

	updateVisibleChunks(playerXPos, playerZPos);
}

ChunkHandler::~ChunkHandler()
{
	//delete[] translations;
}

void ChunkHandler::updateVisibleChunks(float playerXPos, float playerZPos)
{
	playerLastX = playerXPos;
	playerLastZ = playerZPos;

	visibleChunks.clear();

	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkXIndex = std::floor(xPos / chunkSize);
	int chunkZIndex = std::floor(zPos / chunkSize);

	currentChunkIndex = std::pair<int, int>(chunkXIndex, chunkZIndex);

	int chunkVisibility = (visibleChunkSide - 1) / 2;

	for (int i = chunkXIndex - chunkVisibility; i < (chunkXIndex + visibleChunkSide) - chunkVisibility; i++)
	{
		for (int j = chunkZIndex - chunkVisibility; j < (chunkZIndex + visibleChunkSide) - chunkVisibility; j++)
		{
			std::map<std::pair<int, int>, int>::iterator mapIt = worldmap.find(std::pair<int, int>(i, j));
			if (mapIt != worldmap.end())
			{
				int chunkIndex = mapIt->second;
				visibleChunks.push_back(chunkIndex);
			}
			else
			{
				// If chunk is not found, generate a new chunk
 				generateChunk(i, j);
				int chunkIndex = worldmap.at(std::pair<int, int>(i, j));
				visibleChunks.push_back(chunkIndex);
			}
		}
	}

	//populateTranslationsArray();
}

void ChunkHandler::updatePlayerPosition(float playerXPos, float playerZPos)
{
	playerLastX = playerXPos;
	playerLastZ = playerZPos;

	// Player's rounded position
	int xPos = static_cast<int>(playerXPos);
	int zPos = static_cast<int>(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	int chunkXIndex = std::floor(xPos / chunkSize);
	int chunkZIndex = std::floor(zPos / chunkSize);

	// If player has changed chunk, update the visible chunks
	if (currentChunkIndex.first != chunkXIndex || currentChunkIndex.second != chunkZIndex)
	{
		updateVisibleChunks(playerXPos, playerZPos);
	}
}

float ChunkHandler::getHeightValue(float playerXPos, float playerZPos)
{
	playerLastX = playerXPos;
	playerLastZ = playerZPos;

	// Player's rounded position
	// Using std::floor instead of static_cast<int> because 
	// it rounds numbers to negative infinity, while the cast
	// rounds numbers to 0
	int xPos = std::floor(playerXPos);
	int zPos = std::floor(playerZPos);

	// Absolute x and z positions of the chunk directly beneath the player
	// Create a float chunkSize so I can have a decimal result in the division
	// Before calling floor()
	float chunkSizef = chunkSize;
	int chunkXIndex = std::floor(xPos / chunkSizef);
	int chunkZIndex = std::floor(zPos / chunkSizef);

	int chunkIndex = worldmap.at(std::pair<int, int>(chunkXIndex, chunkZIndex));
	
	// If coordinate is below zero, get the module of the position and
	// sum chunkSize to it to get the real position in the blocksHeight array
	xPos = (xPos < 0) ? (chunkSize + (xPos % 16)) : xPos;
	zPos = (zPos < 0) ? (chunkSize + (zPos % 16)) : zPos;

	return chunks.at(chunkIndex).blocksHeight[xPos % chunkSize][zPos % chunkSize];
}

float ChunkHandler::getVisibleChunkSide()
{
	return visibleChunkSide;
}

void ChunkHandler::setVisibleChunkSid(int newVisibleChunkSide)
{
	visibleChunkSide = newVisibleChunkSide;

	numberOfVisibleCubes = chunkSize * chunkSize * visibleChunkSide * visibleChunkSide;

	//translations = new glm::mat4[numberOfVisibleCubes];

	updateVisibleChunks(playerLastX, playerLastZ);
}

void ChunkHandler::generateChunk(int chunkXIndex, int chunkZIndex)
{
	//worldmap.insert(std::pair<std::pair<int, int>, int>(std::pair<int, int>(chunkXIndex, chunkZIndex), chunks.size()));
	worldmap[std::pair<int, int>(chunkXIndex, chunkZIndex)] = chunks.size();
	chunks.push_back(Chunk(chunkXIndex * chunkSize, chunkZIndex * chunkSize));

	// At this point I know which chunk to generate
	// Now I enter the chunk and generate all its blocks
	for (int x = 0; x < chunkSize; x++)
	{
		for (int z = 0; z < chunkSize; z++)
		{
			// Get absolute positions to use in the noise function
			int absoluteX = (chunkXIndex * chunkSize) + x;
			int absoluteY = (chunkZIndex * chunkSize) + z;

			// Normalize absolute x and z positions to 0-1 range
			GLfloat perlinX = bmath::norm(absoluteX, 0, 255);
			GLfloat perlinZ = bmath::norm(absoluteY, 0, 255);

			GLfloat perlinValue = this->perlin.noise(perlinX, perlinZ, 0.0f);

			// Map absolute x and z positions back to 0-255 range
			GLfloat mappedValue = std::floor(bmath::map(perlinValue, 0.0f, 1.0f, 0.0f, 255.0f));

			// Add height value to block
			chunks.back().blocksHeight[x][z] = mappedValue;
			// Add a random block type value
			int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));
			chunks.back().blocksType[x][z] = (randNum < 50) ? 0 : 1;
		}
	}
}

void ChunkHandler::generateInitialChunks(int chunkX, int chunkZ)
{

}

void ChunkHandler::populateTranslationsArray()
{
	// Used for instancing
	int blockX;
	int blockY;
	int blockZ;
	int blockType;
	int translationIndex = 0;

	glm::mat4 model;

	Chunk * chunkPtr;

	for (int i = 0; i < visibleChunks.size(); i++)
	{
		chunkPtr = &chunks.at(visibleChunks.at(i));

		for (int x = 0; x < chunkSize; x++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));

				blockX = chunkPtr->x + x + 0.5f;
				blockY = chunkPtr->blocksHeight[x][z];
				blockZ = chunkPtr->z + z + 0.5f;
				blockType = chunkPtr->blocksType[x][z];

				model = glm::translate(model, glm::vec3(blockX, blockY, blockZ));

				translations[translationIndex] = model;

				translationIndex++;
			}
		}
	}
}