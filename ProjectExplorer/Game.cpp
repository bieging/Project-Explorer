#include "Game.h"

Game::Game(GLuint width, GLuint height) : gs(WELCOME), keys(), actions(), screenWidth(width), screenHeight(height)
{
}

Game::~Game()
{
}

// Initialization
void Game::init(GLFWwindow* windowptr)
{
	this->windowptr = windowptr;

	camera = Camera(initialPosition);
	playerPos = initialPosition;

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

	frustumHandler = new FrustumC();

	// Initialize player's Y position
	GLint heightValue = (chunkHandler->getHeightValue(playerPos.x, playerPos.z));
	playerPos.y = heightValue;
	playerLastY = playerPos.y;

	// Load textures
	stoneTexID = loadTexture("Textures/rockWire.png");
	grassTexID = loadTexture("Textures/grassRealWire.png");
	sandTexID = loadTexture("Textures/sandWire.png");
	waterTexID = loadTexture("Textures/water.png");
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
	lbFPS = Label("FPS: " + std::to_string(30), 25.0f, 570.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerX = Label("Player X: " + std::to_string(camera.Position.x), 25.0f, 550.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerY = Label("Player Y: " + std::to_string(camera.Position.y), 25.0f, 530.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbPlayerZ = Label("Player Z: " + std::to_string(camera.Position.z), 25.0f, 510.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbYVelocity = Label("Y Velocity: " + std::to_string(gravityVelocity), 25.0f, 470.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbMouseX = Label("Mouse X: " + std::to_string(lastX), 25.0f, 450.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbMouseY = Label("Mouse Y: " + std::to_string(lastY), 25.0f, 430.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbWorldSize = Label("World Size: " + std::to_string(chunkHandler->chunks.size()), 25.0f, 410.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbVisibleChunks = Label("Visible Chunks: " + std::to_string(chunkHandler->visibleChunks.size()), 25.0f, 390.0f, 0.3f, fontArial->getCharacterSet(PLAIN));
	lbNearChunks = Label("Near Chunks: " + std::to_string(chunkHandler->nearChunks.size()), 25.0f, 370.0f, 0.3f, fontArial->getCharacterSet(PLAIN));

	// Add Information Labels to their Label vector
	informationLabels.push_back(&lbFPS);
	informationLabels.push_back(&lbPlayerX);
	informationLabels.push_back(&lbPlayerY);
	informationLabels.push_back(&lbPlayerZ);
	informationLabels.push_back(&lbYVelocity);
	informationLabels.push_back(&lbMouseX);
	informationLabels.push_back(&lbMouseY);
	informationLabels.push_back(&lbWorldSize);
	informationLabels.push_back(&lbVisibleChunks);
	informationLabels.push_back(&lbNearChunks);

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
	this->dt = dt;

	// Update FPS label with new FPS values
	lbFPS.setText("FPS: " + std::to_string(fps));

	// Get new camera view
	view = camera.GetViewMatrix();
	projection = glm::perspective(camera.Zoom, (float)this->screenHeight / (float)screenHeight, 0.1f, 100.0f);
}

void Game::updateUI()
{
	lbYVelocity.setText("Y Velocity: " + std::to_string(gravityVelocity));
}

void Game::treatInputs(float dt)
{
	treatPlayerMovementKeys(dt);
	treatGameControlKeys(dt);
	treatStateChangingKeys(dt);
}

void Game::treatKeyboard(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (gs == ACTIVE || gs == INFORMATION)
		{
			switch (pgs)
			{
			case GRAVITY:
				if (playerJumpEnable)
				{
					spacePressed = true;
					playerJumpEnable = false;
				}
				break;

			case FIXED_H_FLY:
				if (keys[GLFW_KEY_LEFT_SHIFT] &&
					actions[GLFW_KEY_W] == INPUT_RELEASED)
				{
					flyAscend = true;
					flyDescend = false;
				}
				else
				{
					flyAscend = false;
					flyDescend = true;
				}
				break;

			case FREE_FLY:
				break;

			default:
				break;
			}
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		if (gs == ACTIVE || gs == INFORMATION)
		{
			switch (pgs)
			{
			case GRAVITY:
				spacePressed = false;
				break;

			case FIXED_H_FLY:
				flyAscend = false;
				flyDescend = false;
				break;

			case FREE_FLY:
				break;

			default:
				break;
			}
		}
	}

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
		actions[key] = INPUT_PRESSED;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
		actions[key] = INPUT_RELEASED;
	}
}

void Game::treatMouseMovement(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	// Update mouse position labels with new position values
	lbMouseX.setText("Mouse X: " + std::to_string(lastX));
	lbMouseY.setText("Mouse Y: " + std::to_string(lastY));

	if (gs == ACTIVE || gs == INFORMATION)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		checkUICollision();
	}
}

void Game::treatMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (gs == ACTIVE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			mouseClicked = !mouseClicked;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			mouseClicked = !mouseClicked;
		}
	}
	if (gs == MENU)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				if (menuLabels.at(i)->colliding)
				{
					menuLabels.at(i)->leftMouseClickFunction();
				}
			}
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				if (menuLabels.at(i)->colliding)
				{
					menuLabels.at(i)->rightMouseClickFunction();
				}
			}
		}
	}
}

void Game::treatMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Game::treatPlayerMovementKeys(float dt)
{
	GLfloat lastCameraX = camera.Position.x;
	GLfloat lastCameraZ = camera.Position.z;

	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(FORWARD, dt * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(FORWARD, dt);
	}
	if (keys[GLFW_KEY_S])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(BACKWARD, dt * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(BACKWARD, dt);
	}
	if (keys[GLFW_KEY_A])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(LEFT, dt * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(LEFT, dt);
	}
	if (keys[GLFW_KEY_D])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT] && actions[GLFW_KEY_W] == INPUT_PRESSED)
			camera.ProcessKeyboard(RIGHT, dt * runSpeedMultiplier);
		else
			camera.ProcessKeyboard(RIGHT, dt);
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
		//lbWorldSize.setText("World Size: " + std::to_string(chunkHandler->chunks.size()));
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
	if (keys[GLFW_KEY_KP_ADD] && actions[GLFW_KEY_T] == INPUT_PRESSED)
	{
		chunkHandler->nearChunkSide += 2;
		chunkHandler->updateVisibleChunks(playerPos.x, playerPos.z);
	}

	if (keys[GLFW_KEY_KP_SUBTRACT] && actions[GLFW_KEY_T] == INPUT_PRESSED)
	{
		chunkHandler->nearChunkSide -= 2;
		chunkHandler->updateVisibleChunks(playerPos.x, playerPos.z);
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
			std::cout << "Switched to ACTIVE game state" << std::endl;
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
			std::cout << "Switched to MENU game state" << std::endl;
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
			std::cout << "Switched to INFORMATION game state" << std::endl;
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
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// Mouse Picking
	if (mouseClicked)
	{
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned char result[4];

		glReadPixels(400, 400, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &result);

		std::cout << std::to_string(result[0]) << std::endl;
	}

	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw objects
	view = camera.GetViewMatrix();
	GLfloat projectionRatio = (float)screenHeight / (float)screenHeight;
	projection = glm::perspective(camera.Zoom, projectionRatio, nearDistance, farDistance);

	frustumHandler->setCamInternals(camera.Zoom, projectionRatio, nearDistance, farDistance, camera.Zoom);
	
	// Calculates the direction ray
	//glm::vec3 viewDirection = camera.Position - (camera.Position + camera.Front);
	//GLfloat nearHeight = 2 * std::tan(camera.Zoom / 2) * nearDistance;
	//GLfloat farHeight  = 2 * std::tan(camera.Zoom / 2) *  farDistance;

	//GLfloat nearWidth = nearHeight * projectionRatio;
	//GLfloat farWidth  = farHeight  * projectionRatio;



	//// fc - Point that crosses the far plane in the direction ray
	//glm::vec3 fc = camera.Position + (viewDirection * farDistance);
	//glm::vec3 ftl = fc + (camera.Up * (farHeight / 2)) - (camera.Right * (farWidth / 2));
	//glm::vec3 fbl = ftl - glm::vec3(0.0f, 0.0f, farHeight);
	//glm::vec3 ftr = fc + (camera.Up * (farHeight / 2)) + (camera.Right * (farWidth / 2));
	//glm::vec3 fbr = ftr - glm::vec3(0.0f, 0.0f, farHeight);

	//glm::vec3 fbl = fc - (camera.Up * (farHeight / 2)) - (camera.Right * (farWidth / 2));
	//glm::vec3 fbr = fc - (camera.Up * (farHeight / 2)) + (camera.Right * (farWidth / 2));

	//glm::vec3 cbl = camera.Position + glm::vec3()
	//glm::vec3 cbr

	// Calculates the right plane
	//glm::vec3 v = ftl - fbl;

	//// fc - Point that crosses the far plane in the direction ray
	//glm::vec3 nc = camera.Position + (viewDirection * nearDistance);
	//glm::vec3 ntl = nc + (camera.Up * (nearHeight / 2)) - (camera.Right * (nearWidth / 2));
	//glm::vec3 ntr = nc + (camera.Up * (nearHeight / 2)) + (camera.Right * (nearWidth / 2));
	//glm::vec3 nbl = nc - (camera.Up * (nearHeight / 2)) - (camera.Right * (nearWidth / 2));
	//glm::vec3 nbr = nc - (camera.Up * (nearHeight / 2)) + (camera.Right * (nearWidth / 2));

	//glm::vec3 a = (nc + camera.Right * (nearWidth / 2)) - camera.Position;
	//glm::normalize(a);
	//glm::vec3 rightNormal = camera.Up * a;

	shaderGEO.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(shaderGEO.Program, "ambStr"), 1);

	shaderLAMP.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(shaderLAMP.Program, "ambStr"), 1);

	if (gs == ACTIVE)
	{
		// Used to render as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		geometryRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		geometryRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);
		geometryRender(shaderGEO, sandTexID, sandColor, SAND_ID);

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
		geometryRender(shaderGEO, sandTexID, sandColor, SAND_ID);

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
		geometryRender(shaderGEO, sandTexID, sandColor, SAND_ID);

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
		geometryRender(shaderGEO, sandTexID, sandColor, SAND_ID);

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
	glfwSwapBuffers(windowptr);
}

void Game::geometryRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest)
{
	int blockX;
	int blockY;
	int blockZ;
	int blockType;

	Chunk * chunkPtr;

	GLfloat projectionRatio = (float)screenHeight / (float)screenHeight;

	//float distance = bmath::distPT2LN(camera.Position, normDirection, lightPos);
	//lbWorldSize.setText("Distance Player to Sun: " + std::to_string(camera.Yaw));

	// Draw sun
	shaderLAMP.Use();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(lightVAO);

	GLfloat timeValue = glfwGetTime();

	GLfloat sunX = sin(timeValue / 10);
	GLfloat sunY = cos(timeValue / 10);

	sunX = bmath::map(sunX, 0, 1, 0, 500);
	sunY = bmath::map(sunY, 0, 1, 0, 500);

	lightPos = glm::vec3(sunX + playerPos.x, sunY + playerPos.y, playerPos.z);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, lightPos);
	//model = glm::translate(model, ftl);
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
		
	frustumHandler->setCamDef(camera.Position, (camera.Position + camera.Front), camera.Up, camera.Right);

	int visibleChunks = 0;

	for (int i = 0; i < chunkHandler->nearChunks.size(); i++)
	{
		chunkPtr = &chunkHandler->chunks.at(chunkHandler->nearChunks.at(i));

		if (frustumHandler->pointInFrustum(glm::vec3(chunkPtr->x, camera.Position.y, chunkPtr->z)))
		{
			visibleChunks++;

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

					// Frustum culling test
					//glm::vec3 blockPos = glm::vec3(blockX, blockY, blockZ);

					//if (bmath::distPT2LN(camera.Position, normDirection, blockPos) < maxRadious)
					//{
					//	if (blockType == blockTypeRequest)
					//	{
					//		glm::mat4 model;
					//		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
					//		model = glm::translate(model, glm::vec3(blockX + 0.5f, blockY, blockZ + 0.5f));
					//		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
					//		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
					//		glDrawArrays(GL_TRIANGLES, 0, 36);
					//	}
					//}
				}
			}
		}
	}

	lbVisibleChunks.setText("Visible Chunks: " + std::to_string(visibleChunks));

	visibleChunks = 0;

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
	//glfwSetWindowShouldClose(window, GL_TRUE);
}