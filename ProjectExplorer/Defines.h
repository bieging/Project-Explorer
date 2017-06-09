#pragma once

#define GRASS_ID 0
#define STONE_ID 1

const float maxGravityVelocity = 0.8f;
const float gravityAcceleration = 0.5f;

const float playerHeight		= 2.2f;
const float playerSpeed			= 3.0f;
	  bool	playerJumpEnable	= true;
const int	playerJumpTime		= 13;
const float playerJumpAcceleration = 0.75f;

const float sunSize = 35.0f;

const glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(1.0f, 250.0f, 1.0f);

glm::mat4 view;
glm::mat4 projection;