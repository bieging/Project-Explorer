// Std. Includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLFW
#include <c:/opengl/GLFW/glfw3.h>

// IRRKLANG
#include <c:/opengl/IRRKLANG/irrKlang.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "Font.h"
#include "Label.h"

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>
#include <c:/opengl/glm/gtc/matrix_transform.hpp>
#include <c:/opengl/glm/gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other Libs
#include <c:/opengl/SOIL/SOIL.h>

//#define glm::vec3(1.0f, 1.0f, 1.0f) WHITE

using namespace irrklang;

ISoundEngine *SoundEngine = createIrrKlangDevice();

//Font fontArial;

// Properties
GLuint screenWidth = 800, screenHeight = 600;
GLuint mapSideSize = 50;
GLuint mapBorderSize = 25;

int randomMin = 0;
int randomMax = 100;

GLint textXSize = 0;

int fps = 0;
double fpsCounter = 0;
const int fpsCounterLimit = 1000;
int frames = 0;

bool spacePressed = false;
bool shiftPressed = false;
bool spaceReleased = false;
bool shiftReleased = false;

bool jumpEnable = true;
int jumpTime = 7;
int jumpTimeCounter = jumpTime;

GLfloat runSpeedMultiplier = 2.5f;

bool flyAscend = false;
bool flyDescend = false;

bool renderInformationText = false;
bool mouseFlag = false;

enum PLAYER_GRAVITY_STATE
{
	GRAVITY,
	FIXED_H_FLY,
	FREE_FLY
};

enum GAME_STATE
{
	WELCOME,
	START,
	MENU,
	ACTIVE,
	INFORMATION
};

PLAYER_GRAVITY_STATE PGS = GRAVITY;
GAME_STATE gs = ACTIVE;

/// Holds all state information relevant to a character as loaded using FreeType
//struct Character {
//	GLuint TextureID;   // ID handle of the glyph texture
//	glm::ivec2 Size;    // Size of glyph
//	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
//	GLuint Advance;    // Horizontal offset to advance to next glyph
//};
//
//std::map<GLchar, Character> Characters;
//std::map<GLchar, Character> CharactersBold;

// Player
GLfloat gravityVelocity = 0.05f;
GLfloat maxGravityVelocity = 0.3;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void Do_Movement();
void updatePlayerVelocity(GLfloat dt);

void pgsGravity(GLfloat dt);
void pgsFixedHFly(GLfloat dt);
void pgsFreeFly(GLfloat dt);

//void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
GLuint loadTexture(GLchar* path);
void initRenderData();
//void initTextRenderData();
void RawRender(Shader &shader, GLint textureID, glm::vec3 color, std::vector<glm::vec3> blocks);
void Render();

//std::tuple<GLfloat, GLfloat> uiTextDimension(std::string text, GLfloat x, GLfloat y, GLfloat scale);
void uiCollision();
bool checkUiCollision(std::string text);

void initializeWorldVectors();

// Camera
Camera camera(glm::vec3(25.0f, 1.5f, 25.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

glm::vec3 playerPos(glm::vec3(25.0f, 25.0f, 25.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

std::vector<glm::vec3> grassBlocks;
std::vector<glm::vec3> stoneBlocks;

std::vector<glm::vec3> heightMapPos;
std::vector<GLint> heightValue;

GLuint cubeVAO, cubeVBO;
GLuint textVAO, textVBO;

GLuint stoneTexID;
GLuint grassTexID;

glm::vec3 stoneColor  = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 grassColor = glm::vec3(0.2, 0.7, 0.2);

// Shaders
Shader shaderGEO;
Shader shaderTXT;

// Global lighting vars
GLfloat timeValue;
GLfloat strength;

// GLFW Window
GLFWwindow* window;

// Strings
std::vector<Label> welcomeLabels;
//std::vector<std::string> informationStrings;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(screenWidth, screenHeight, "Depth Testing", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCharCallback(window, character_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// Setup and compile our shaders
	shaderGEO.init("Shaders/depth.vs", "Shaders/depth.frag");
	shaderTXT.init("Shaders/text.vs", "Shaders/text.frag");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	
	shaderTXT.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderTXT.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Play audio
	//SoundEngine->play2D("Audio/breakout.mp3", GL_TRUE);

#pragma region "object_initialization"
	// Set the object data (buffers, vertex attributes)
	
	initializeWorldVectors();

	Font fontArial("Fonts", "arial");

	Label lbPlayerXPosition("Welcome to Project Explorer", 100.0f, 300.0f, 1.0f, fontArial.getCharacterSet(PLAIN));

	welcomeLabels.push_back(lbPlayerXPosition);

	//initTextRenderData();

	initRenderData();

	// Load textures
	stoneTexID = loadTexture("Textures/rock.png");
	grassTexID  = loadTexture("Textures/grassReal.png");
#pragma endregion

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		auto start = std::chrono::high_resolution_clock::now();
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		timeValue = glfwGetTime();
		strength = abs(sin(timeValue / 20));

		if (strength <= 0.1)
			strength = 0.1;

		updatePlayerVelocity(deltaTime);

		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

		// Clear the colorbuffer
		glClearColor(strength, strength, strength, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Draw objects
		shaderGEO.Use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1f(glGetUniformLocation(shaderGEO.Program, "ambStr"), strength);

		Render();
		//lbPlayerXPosition.render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));

		auto finish = std::chrono::high_resolution_clock::now();
		fpsCounter += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
		frames++;
		
		if (fpsCounter > fpsCounterLimit)
		{
			fps = frames;

			fpsCounter -= 1000;
			frames = 0;
		}
	}

	glfwTerminate();
	return 0;
}

void updatePlayerVelocity(GLfloat dt)
{
	switch (PGS)
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

void pgsGravity(GLfloat dt)
{
	if (spacePressed == false)
	{
		if (spaceReleased)
		{
			// Checks if player stopped ascending after releasing space key
			if (gravityVelocity >= 0)
			{
				// This softly stops the player after stops the jump command
				gravityVelocity -= 0.75f * dt;
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
			GLint heightI = std::find(heightMapPos.begin(), heightMapPos.end(), glm::vec3(floor(playerPos.x), 0.0, floor(playerPos.z))) - heightMapPos.begin();

			if (playerPos.y > heightValue.at(heightI))
			{
				if (gravityVelocity < maxGravityVelocity)
				{
					gravityVelocity += 0.2 * dt;
				}
				else
				{
					gravityVelocity = maxGravityVelocity;
				}

				playerPos.y -= gravityVelocity;
			}
			else
			{
				//if (playerPos.y == heightValue.at(heightI))
				//{
					playerPos.y = heightValue.at(heightI);

					gravityVelocity = 0.05f;

					jumpEnable = true;
				//}

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

			jumpTimeCounter = jumpTime;
		}
	}

	camera.Position.y = playerPos.y + 1.5;
}

void pgsFixedHFly(GLfloat dt)
{
	if (flyAscend)
	{
		playerPos.y += 2 * dt;
	}
	else if (flyDescend)
	{
		playerPos.y -= 2 * dt;
	}

	camera.Position.y = playerPos.y + 1.5;
}

void pgsFreeFly(GLfloat dt)
{
}

// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
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

#pragma region Initialization

void initializeWorldVectors()
{
	grassBlocks.clear();
	stoneBlocks.clear();
	heightMapPos.clear();
	heightValue.clear();

	for (int i = 25; i < mapSideSize + mapBorderSize; i++)
	{
		for (int j = 25; j < mapSideSize + mapBorderSize; j++)
		{
			int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));

			if (randNum < 50)
			{
				grassBlocks.push_back(glm::vec3(i + 0.5f, 0.0, j + 0.5f));
			}
			else
			{
				stoneBlocks.push_back(glm::vec3(i + 0.5f, 0.0, j + 0.5f));
			}
		}
	}

	for (int i = 0; i < mapSideSize + ((2 * mapBorderSize) - 1); i++)
	{
		for (int j = 0; j < mapSideSize + (2 * mapBorderSize - 1); j++)
		{
			heightMapPos.push_back(glm::vec3(i, 0.0, j));

			if ((j >= 25 && i >= 25) && (j <= 74 && i <= 74))
				heightValue.push_back(0.0);
			else
				heightValue.push_back(-100.0);
		}
	}
}

#pragma endregion

#pragma region "Render"

void initRenderData()
{
	GLfloat cubeVertices[] =
	{
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// Setup cube VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void RawRender(Shader &shader, GLint textureID, glm::vec3 color, std::vector<glm::vec3> blocks)
{
	shader.Use();
	// Floor Cubes
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, textureID);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (sampler used in fragment is set to 0 as well as default)		

	GLint cubesColor = glGetUniformLocation(shader.Program, "inColor");
	glUniform3f(cubesColor, color.r, color.g, color.b);

	for (int i = 0; i < blocks.size(); i++)
	{
		glm::mat4 model;
		model = glm::translate(model, blocks.at(i));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	glBindVertexArray(0);
}

void Render()
{
	if (gs == ACTIVE)
	{
		RawRender(shaderGEO, grassTexID, grassColor, grassBlocks);
		RawRender(shaderGEO, stoneTexID, stoneColor, stoneBlocks);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label>::iterator lbIt = welcomeLabels.begin();
		GLint i = 0;

		for (; lbIt < welcomeLabels.end(); lbIt++, i++)
		{
			welcomeLabels.at(i).render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		//for each (Label lb in welcomeLabels)
		//{
		//	lb.render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		//}

		//if (renderInformationText == true)
		//{
		//	//lbPlayerXPosition.render();

		//	mouseFlag = (lastX > 400) ? false : true;

		//	//todo - create textColor var
		//	//if (!mouseFlag)
		//	//{
		//	//	RenderText(shaderTXT, "FPS: " + std::to_string(fps), 25.0f, 570.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player X: " + std::to_string(camera.Position.x), 25.0f, 550.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player Y: " + std::to_string(camera.Position.y), 25.0f, 530.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player Z: " + std::to_string(camera.Position.z), 25.0f, 510.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Ambient Light Strength: " + std::to_string(strength), 25.0f, 490.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Y Velocity: " + std::to_string(gravityVelocity), 25.0f, 470.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Mouse X: " + std::to_string(lastX), 25.0f, 450.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Mouse Y: " + std::to_string(lastY), 25.0f, 430.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//}
		//	//else
		//	//{
		//	//	RenderText(shaderTXT, "FPS: " + std::to_string(fps), 25.0f, 570.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player X: " + std::to_string(camera.Position.x), 25.0f, 550.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player Y: " + std::to_string(camera.Position.y), 25.0f, 530.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Player Z: " + std::to_string(camera.Position.z), 25.0f, 510.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Ambient Light Strength: " + std::to_string(strength), 25.0f, 490.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Y Velocity: " + std::to_string(gravityVelocity), 25.0f, 470.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Mouse X: " + std::to_string(lastX), 25.0f, 450.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//	RenderText(shaderTXT, "Mouse Y: " + std::to_string(lastY), 25.0f, 430.0f, 0.3f, glm::vec3(1.0, 0.0f, 0.0f));
		//	//}
		//}

	}
	else if (gs == WELCOME)
	{
		RawRender(shaderGEO, grassTexID, grassColor, grassBlocks);
		RawRender(shaderGEO, stoneTexID, stoneColor, stoneBlocks);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//uiTextDimension
		// todo - get text dimension and render the text on position based on dimension
		//RenderText(shaderTXT, "Welcome to Project Explorer", 100.0f, 300.0f, 1.0f, glm::vec3(1.0, 1.0f, 1.0f));

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	else if (gs == START)
	{
		RawRender(shaderGEO, grassTexID, grassColor, grassBlocks);
		RawRender(shaderGEO, stoneTexID, stoneColor, stoneBlocks);
	}

	// Swap the buffers
	glfwSwapBuffers(window);
}

void uiCollision()
{
	if (gs == WELCOME)
	{
		if (checkUiCollision("Welcome to Project Explorer"))
		{
			std::cout << "True" << std::endl;
		}
	}
	else if (gs == INFORMATION)
	{
		//if (checkUiCollision(text))
	}
}

bool checkUiCollision(std::string text)
{
	//GLfloat width, height;

	//std::tie(width, height) = uiTextDimension("Welcome to Project Explorer", 0, 0, 1.0f);

	//if (lastX >= 100.0f && lastX <= (100.0f + width) &&
	//	lastY >= 300.0f && lastY <= (300.0f + height))
	//{
	//	return true;
	//}

	return false;
}

#pragma endregion

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		if (shiftPressed)	camera.ProcessKeyboard(FORWARD, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		if (shiftPressed)	camera.ProcessKeyboard(BACKWARD, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		if (shiftPressed)	camera.ProcessKeyboard(LEFT, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		if (shiftPressed)	camera.ProcessKeyboard(RIGHT, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	playerPos.x = camera.Position.x;
	playerPos.z = camera.Position.z;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		renderInformationText = (renderInformationText) ? false : true;
		// Options
		if (renderInformationText)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		playerPos = glm::vec3(25.0, 25.0, 25.0);
		initializeWorldVectors();
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		switch (PGS)
		{
			case GRAVITY:
				PGS = FIXED_H_FLY;
				break;
			case FIXED_H_FLY:
				PGS = FREE_FLY;
				break;
			case FREE_FLY:
				PGS = GRAVITY;
				break;
			default:
				break;
		}
	}
	
	if		(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS  )	shiftPressed = true;
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)	shiftPressed = false;
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		switch (PGS)
		{
			case GRAVITY:
				if (jumpEnable)
				{
					spacePressed = true;
					jumpEnable = false;
				}
				break;

			case FIXED_H_FLY:
				if (!shiftPressed)
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
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		switch (PGS)
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

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

	if (gs == ACTIVE)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		uiCollision();
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	//char c = codepoint;
	//std::cout << c;
}

#pragma endregion