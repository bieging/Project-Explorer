#ifndef BMATH_H
#define BMATH_H

//#pragma once

#include <string>
#include <limits>

// GLM Includes
#include "c:/opengl/GLEW/glew.h"
#include "c:/opengl/GLM/glm.hpp"
#include "c:/opengl/GLM/gtc/matrix_transform.hpp"

// This is my math library, or Bieging Math

namespace bmath
{
	// todo - create comment for this
	float norm(float value, float start, float stop);

	// todo - create comment for this
	float map(float value,
		float start1, float stop1,
		float start2, float stop2);

	float distPT2LN(glm::vec3 linePoint, glm::vec3 lineVector, glm::vec3 point);
}

#endif // !BMATH_H