#include "Game.h"

// Sound
ISoundEngine* SoundEngine = createIrrKlangDevice();


// Fonts
Font* fontArial;

// Chunks Handler
ChunkHandler* chunkHandler;

Game::Game(GLuint width, GLuint height) : gs(WELCOME), keys(), actions(), width(width), height(height)
{
}

Game::~Game()
{
}

// Initialization
void Game::init(GLFWwindow* windowptr)
{
	// Setup and compile our shaders
	shaderGEO.init("Shaders/geo.vs", "Shaders/geo.frag");
	shaderLAMP.init("Shaders/lamp.vs", "Shaders/lamp.frag");
	shaderTXT.init("Shaders/text.vs", "Shaders/text.frag");

	shaderTXT.Use();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	glUniformMatrix4fv(glGetUniformLocation(shaderTXT.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Load fonts
	fontArial = new Font("Fonts", "arial");

	// Create map and heightMap
	initializeWorldmap();

	// Configure all interface related objects
	initializeUI();

	// Initializes all vertices, VAOs and VBOs related to blocks
	initRenderData();

	// Initialize player's Y position
	GLint heightValue = (chunkHandler->getHeightValue(playerPos.x, playerPos.z));
	playerPos.y = heightValue;
	playerLastY = playerPos.y;

	// Load textures
	stoneTexID = loadTexture("Textures/rockWire.png");
	grassTexID = loadTexture("Textures/grassRealWire.png");
}

void Game::initRenderData()
{
	std::cout << "Initializing render data, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	GLfloat cubeVertices[] =
	{
		// Positions          // Tex	   // Normals
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the correct data.
	// set the vertex attributes (only position data for our lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	auto finish = std::chrono::high_resolution_clock::now();
	__int64 initializeTime = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	std::cout << "Render data took: " << std::to_string(initializeTime) << " microseconds to initialize" << std::endl;
	std::cout << std::endl;
}

void Game::initializeWorldmap()
{
	std::cout << "Initializing map, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	int seed;
	std::cout << "Please enter a seed number to generate your terrain: " << std::endl;
	std::cout << "Enter \"n\" to initialize with standard seed" << std::endl;
	std::string seedInput = "";
	std::cin >> seedInput;
	if (seedInput != "n")
	{
		seed = std::stoi(seedInput);
	}

	chunkHandler = new ChunkHandler(playerPos.x, playerPos.z, seed);

	auto finish = std::chrono::high_resolution_clock::now();
	__int64 initializeTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
	std::cout << "Map took: " << std::to_string(initializeTime) << " miliseconds to initialize" << std::endl;
	std::cout << std::endl;
}

void Game::initializeUI()
{
	std::cout << "Initializing UI, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	// Create Welcome Label
	lbWelcome = Label("Welcome to Project Explorer", 100.0f, 300.0f, 1.0f, fontArial->getCharacterSet(PLAIN));

	// Add Welcome Labels to their Label vector
	welcomeLabels.push_back(&lbWelcome);

	// Create Information Labels
	lbFPS = Label("FPS: " + std::to_string(fps), 25.0f, 570.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerX = Label("Player X: " + std::to_string(camera.Position.x), 25.0f, 550.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerY = Label("Player Y: " + std::to_string(camera.Position.y), 25.0f, 530.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerZ = Label("Player Z: " + std::to_string(camera.Position.z), 25.0f, 510.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbLightStrength = Label("Ambient Light Strength: " + std::to_string(strength), 25.0f, 490.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbYVelocity = Label("Y Velocity: " + std::to_string(gravityVelocity), 25.0f, 470.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbMouseX = Label("Mouse X: " + std::to_string(lastX), 25.0f, 450.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbMouseY = Label("Mouse Y: " + std::to_string(lastY), 25.0f, 430.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbWorldSize = Label("World Size: " + std::to_string(chunkHandler->chunks.size()), 25.0f, 410.0f, 0.3f, fontArial->getCharacterSet(PLAIN));

	// Add Information Labels to their Label vector
	informationLabels.push_back(&lbFPS);
	informationLabels.push_back(&lbPlayerX);
	informationLabels.push_back(&lbPlayerY);
	informationLabels.push_back(&lbPlayerZ);
	informationLabels.push_back(&lbLightStrength);
	informationLabels.push_back(&lbYVelocity);
	informationLabels.push_back(&lbMouseX);
	informationLabels.push_back(&lbMouseY);
	informationLabels.push_back(&lbWorldSize);

	// Create Menu Labels
	lbSave = Label("Save", 350.0f, 570.0f, 0.5f, fontArial->getCharacterSet(currentCharacterSet));
	lbLoad = Label("Load", 350.0f, 510.0f, 0.5f, fontArial->getCharacterSet(currentCharacterSet));
	lbExit = Label("Exit", 350.0f, 450.0f, 0.5f, fontArial->getCharacterSet(currentCharacterSet));

	// Set function for each button
	lbSave.setLeftMouseClickFunction(lbSave_leftClick);
	lbLoad.setLeftMouseClickFunction(lbLoad_leftClick);
	lbExit.setLeftMouseClickFunction(lbExit_leftClick);

	// Add Menu Labels to their Label vector
	menuLabels.push_back(&lbSave);
	menuLabels.push_back(&lbLoad);
	menuLabels.push_back(&lbExit);

	auto finish = std::chrono::high_resolution_clock::now();
	__int64 initializeTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
	std::cout << "UI took: " << std::to_string(initializeTime) << " miliseconds to initialize" << std::endl;
	std::cout << std::endl;
}

// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint Game::loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

void Game::update(float dt)
{


}

void Game::updateUI()
{
	lbLightStrength.setText("Ambient Light Strength: " + std::to_string(strength));
	lbYVelocity.setText("Y Velocity: " + std::to_string(gravityVelocity));
}

void Game::treatInputs(float dt)
{
	treatPlayerMovementKeys(dt);
	treatGameControlKeys(dt);
	treatStateChangingKeys(dt);
}

void Game::treatPlayerMovementKeys(float dt)
{
	GLfloat lastCameraX = camera.Position.x;
	GLfloat lastCameraZ = camera.Position.z;

	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(FORWARD, deltaTime * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(BACKWARD, deltaTime * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(LEFT, deltaTime * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(RIGHT, deltaTime * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	GLint heightValue = chunkHandler->getHeightValue(camera.Position.x, camera.Position.z);

	// Player Wall Collision Physics Treatment
	// todo - explain what is going on here
	if (heightValue < (camera.Position.y - playerHeight + 0.5f))
	{
		playerPos.x = camera.Position.x;
		playerPos.z = camera.Position.z;

		chunkHandler->updatePlayerPosition(playerPos.x, playerPos.z);

		// Update player position labels with new position values
		lbPlayerX.setText("Player X: " + std::to_string(camera.Position.x));
		lbPlayerY.setText("Player Y: " + std::to_string(camera.Position.y));
		lbPlayerZ.setText("Player Z: " + std::to_string(camera.Position.z));

		// Update world size label in case new chunks were added
		lbWorldSize.setText("World Size: " + std::to_string(chunkHandler->chunks.size()));
	}
	else
	{
		camera.Position.x = lastCameraX;
		camera.Position.z = lastCameraZ;
	}
}

void Game::treatGameControlKeys(float dt)
{
	if (keys[GLFW_KEY_T] && actions[GLFW_KEY_T] == INPUT_PRESSED)
	{
		actions[GLFW_KEY_T] = INPUT_HOLD;

		if (gs == ACTIVE)
		{
			switch (pgs)
			{
			case GRAVITY:
				pgs = FIXED_H_FLY;
				break;
			case FIXED_H_FLY:
				pgs = FREE_FLY;
				break;
			case FREE_FLY:
				pgs = GRAVITY;
				break;
			default:
				break;
			}
		}
	}
}

void Game::treatStateChangingKeys(float dt)
{
	if (keys[GLFW_KEY_ENTER] && actions[GLFW_KEY_ENTER] == INPUT_PRESSED)
	{
		actions[GLFW_KEY_ENTER] = INPUT_HOLD;

		if (gs == WELCOME)
		{
			gs = ACTIVE;
		}
	}

	if (keys[GLFW_KEY_ESCAPE] && actions[GLFW_KEY_ESCAPE] == INPUT_PRESSED)
	{
		actions[GLFW_KEY_ESCAPE] = INPUT_HOLD;

		if (gs != MENU)
		{
			glfwSetInputMode(windowptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursorFree = true;
			lastGameState = gs;
			gs = MENU;
		}
		else
		{
			glfwSetInputMode(windowptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursorFree = false;
			if (lastGameState == gs)
			{
				gs = ACTIVE;
			}
			else
			{
				gs = lastGameState;
			}
		}
	}

	if (keys[GLFW_KEY_F3] && actions[GLFW_KEY_F3] == INPUT_PRESSED)
	{
		actions[GLFW_KEY_F3] = INPUT_HOLD;

		if (gs != INFORMATION)
		{
			lastGameState = gs;
			gs = INFORMATION;
		}
		else
		{
			if (lastGameState == gs)
			{
				gs = ACTIVE;
			}
			else
			{
				gs = lastGameState;
			}
		}
	}

	if (keys[GLFW_KEY_R] && actions[GLFW_KEY_R] == INPUT_PRESSED)
	{
		actions[GLFW_KEY_R] = INPUT_HOLD;

		if (gs == ACTIVE)
		{
			playerPos = glm::vec3(25.0, 25.0, 25.0);
			initializeWorldmap();
		}
	}
}

void Game::checkUICollision()
{
	if (cursorFree)
	{
		if (gs == MENU)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				menuLabels.at(i)->checkCollision(lastX, screenHeight - lastY);
			}
		}
	}
}

void Game::treatPhysics(GLfloat dt)
{
	switch (pgs)
	{
		case GRAVITY:
			pgsGravity(dt);
			break;
		case FIXED_H_FLY:
			pgsFixedHFly(dt);
			break;
		case FREE_FLY:
			pgsFreeFly(dt);
			break;
		default:
			break;
	}
}

void Game::pgsGravity(GLfloat dt)
{
	if (spacePressed == false)
	{
		if (spaceReleased)
		{
			// Checks if player stopped ascending after releasing space key
			if (gravityVelocity >= 0)
			{
				// This softly stops the player after stops the jump command
				gravityVelocity -= playerJumpAcceleration * dt;
				playerPos.y += gravityVelocity;
			}
			else
			{
				// Once player stops, let gravity act by setting spaceReleased to false
				spaceReleased = false;
				gravityVelocity = 0;
			}
		}
		else
		{
			GLint heightValue = chunkHandler->getHeightValue(playerPos.x, playerPos.z);

			if (playerPos.y > heightValue)
			{
				if (gravityVelocity < maxGravityVelocity)
				{
					gravityVelocity += gravityAcceleration * dt;
				}
				else
				{
					gravityVelocity = maxGravityVelocity;
				}

				playerPos.y -= gravityVelocity;
			}
			else	// Player touched the ground
			{
				playerPos.y = heightValue;

				gravityVelocity = 0.05f;

				playerJumpEnable = true;
			}
		}
	}
	else
	{
		if (gravityVelocity < maxGravityVelocity)
		{
			gravityVelocity += 0.2 * dt;
		}
		else
		{
			gravityVelocity = maxGravityVelocity;
		}

		playerPos.y += gravityVelocity;

		jumpTimeCounter -= dt;

		if (jumpTimeCounter <= 0)
		{
			spacePressed = false;
			spaceReleased = true;

			jumpTimeCounter = playerJumpTime;
		}
	}

	camera.Position.y = playerPos.y + playerHeight;
}

void Game::pgsFixedHFly(GLfloat dt)
{
	if (flyAscend)
	{
		playerPos.y += 2 * dt;
	}
	else if (flyDescend)
	{
		playerPos.y -= 2 * dt;
	}

	camera.Position.y = playerPos.y + playerHeight;
}

void Game::pgsFreeFly(GLfloat dt)
{

}

void Game::render()
{
	// Draw objects
	view = camera.GetViewMatrix();
	projection = glm::perspective(camera.Zoom, (float)screenHeight / (float)screenHeight, 0.1f, 750.0f);

	shaderGEO.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(shaderGEO.Program, "ambStr"), strength);

	shaderLAMP.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(shaderLAMP.Program, "ambStr"), strength);

	if (gs == ACTIVE)
	{
		// Used to render as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		geometryRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		geometryRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Set OpenGL options
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//std::vector<Label>::iterator lbIt = activeLabels.begin();
		//GLint i = 0;

		//for (; lbIt < activeLabels.end(); lbIt++, i++)
		//{
		//	activeLabels.at(i).render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		//}

		//glDisable(GL_BLEND);
		//glDisable(GL_CULL_FACE);

	}
	else if (gs == WELCOME)
	{
		geometryRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		geometryRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = welcomeLabels.begin();
		GLint i = 0;

		for (; lbIt < welcomeLabels.end(); lbIt++, i++)
		{
			welcomeLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	else if (gs == INFORMATION)
	{
		geometryRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		geometryRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = informationLabels.begin();
		GLint i = 0;

		for (; lbIt < informationLabels.end(); lbIt++, i++)
		{
			informationLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	else if (gs == MENU)
	{
		geometryRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		geometryRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = menuLabels.begin();
		GLint i = 0;

		for (; lbIt < menuLabels.end(); lbIt++, i++)
		{
			menuLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		lbIt = informationLabels.begin();
		i = 0;

		for (; lbIt < informationLabels.end(); lbIt++, i++)
		{
			informationLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}

	// Swap the buffers
	glfwSwapBuffers(window);
}

void Game::geometryRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest)
{
	int blockX;
	int blockY;
	int blockZ;
	int blockType;

	Chunk * chunkPtr;

	// Draw sun
	shaderLAMP.Use();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(lightVAO);

	GLfloat sunX = sin(timeValue / 10);
	GLfloat sunY = cos(timeValue / 10);

	sunX = map(sunX, 0, 1, 0, 500);
	sunY = map(sunY, 0, 1, 0, 500);

	lightPos = glm::vec3(sunX + playerPos.x, sunY + playerPos.y, playerPos.z);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(sunSize));
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shader.Use();
	// Draw worldmap
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, textureID);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (sampler used in fragment is set to 0 as well as default)		

	GLint shObjectColor = glGetUniformLocation(shader.Program, "objectColor");
	glUniform3f(shObjectColor, color.r, color.g, color.b);
	GLint shLightColor = glGetUniformLocation(shader.Program, "lightColor");
	glUniform3f(shLightColor, lightColor.r, lightColor.g, lightColor.b);
	GLint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
	glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

	//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunkHandler.numberOfVisibleCubes);
	//glBindVertexArray(0);

	for (int i = 0; i < chunkHandler->visibleChunks.size(); i++)
	{
		chunkPtr = &chunkHandler->chunks.at(chunkHandler->visibleChunks.at(i));

		for (int x = 0; x < chunkSize; x++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));

				blockX = chunkPtr->x + x;
				blockY = chunkPtr->blocksHeight[x][z];
				blockZ = chunkPtr->z + z;
				blockType = chunkPtr->blocksType[x][z];

				if (blockType == blockTypeRequest)
				{
					glm::mat4 model;
					//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
					model = glm::translate(model, glm::vec3(blockX + 0.5f, blockY, blockZ + 0.5f));
					//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
					glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
	}

	glBindVertexArray(0);
}

// UI Functions
void lbSave_leftClick()
{
	std::cout << "Saved map" << std::endl;
}

void lbLoad_leftClick()
{
	std::cout << "Loaded new map" << std::endl;
}

void lbExit_leftClick()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}