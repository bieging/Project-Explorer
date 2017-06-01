#ifndef CHARACTER_H
#define CHARACTER_H

#pragma once

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

#endif