/* ------------------------------------------------------

 View Frustum - Lighthouse3D

  -----------------------------------------------------*/


#ifndef _FRUSTUMG_
#define _FRUSTUMG_

#ifndef _VEC3_
#include "Vec3.h"
#endif

class Vec3;

#ifndef _PLANE_
#include "Plane.h"
#endif

class Plane;

// GLM Includes
#include "c:/opengl/GLEW/glew.h"
#include "c:/opengl/GLM/glm.hpp"
#include "c:/opengl/GLM/gtc/matrix_transform.hpp"

class FrustumC 
{
public:

	static enum {OUTSIDE, INTERSECT, INSIDE};

	Plane pl[2];

	Vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle,tang;
	
	float farHeight;
	float farWidth;
	float nearHeight;
	float nearWidth;

	FrustumC::FrustumC();
	FrustumC::~FrustumC();

	void FrustumC::setCamInternals(float angle, float ratio, float nearD, float farD, float zoom);
	void FrustumC::setCamDef(glm::vec3 &inp, glm::vec3 &inl, glm::vec3 &inu, glm::vec3 &inr);
	int FrustumC::pointInFrustum(glm::vec3 &inp);

private:
	enum
	{
		LEFT = 0,
		RIGHT
	};
};


#endif