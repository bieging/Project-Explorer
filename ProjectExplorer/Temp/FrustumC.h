#ifndef FRUSTUMC_H
#define FRUSTUMC_H

#pragma once

#include "Camera.h"
#include "Plane.h"

// GLM Includes
#include "c:/opengl/GLEW/glew.h"
#include "c:/opengl/GLM/glm.hpp"
#include "c:/opengl/GLM/gtc/matrix_transform.hpp"

class FrustumC
{
public:
	static enum { OUTSIDE, INTERSECT, INSIDE };

	Plane planes[2];

	glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	FrustumC();
	~FrustumC();

	void setCamInternals(float angle, float ratio, float nearDistance, float farDistance, Camera* camera);
	void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u, float projRatio);
	int pointInFrustum(glm::vec3 &p);
private:
	Camera* camera;

	enum
	{
		LEFT = 0,
		RIGHT
	};
};

#endif // !FRUSTUMC_H