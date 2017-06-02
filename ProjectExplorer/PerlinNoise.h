#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#pragma once

#include <vector>

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>


class PerlinNoise
{
public:
	PerlinNoise();
	PerlinNoise(GLuint seed);
	~PerlinNoise();

	// todo - comment this method
	GLfloat noise(GLfloat x, GLfloat y, GLfloat z);
private:
	GLfloat fade(GLfloat t);
	GLfloat lerp(GLfloat t, GLfloat a, GLfloat b);
	GLfloat grad(GLint hash, GLfloat x, GLfloat y, GLfloat z);
	
	std::vector<GLint> p;
};

#endif // !PERLIN_NOISE_H