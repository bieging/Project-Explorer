/* ------------------------------------------------------

 View Frustum - Lighthouse3D

  -----------------------------------------------------*/

#include "FrustumC.h"
#include <math.h>

#define ANG2RAD 3.14159265358979323846/180.0



FrustumC::FrustumC() {}

FrustumC::~FrustumC() {}

void FrustumC::setCamInternals(float angle, float ratio, float nearD, float farD, float zoom)
{
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	//tang = (float)tan(angle* ANG2RAD * 0.5) ;
	//nh = nearD * tang;
	//nw = nh * ratio; 
	//fh = farD  * tang;
	//fw = fh * ratio;

	farHeight = 2 * std::tan(zoom / 2) *  farD;
	farWidth = farHeight  * this->ratio;
	nearHeight = 2 * std::tan(zoom / 2) *  nearD;
	nearWidth = farHeight  * this->ratio;
}

void FrustumC::setCamDef(glm::vec3 &inp, glm::vec3 &inl, glm::vec3 &inu, glm::vec3 &inr)
{
	Vec3 p = Vec3(inp.x, inp.y, inp.z);
	Vec3 l = Vec3(inl.x, inl.y, inl.z);
	Vec3 u = Vec3(inu.x, inu.y, inu.z);
	Vec3 r = Vec3(inr.x, inr.y, inr.z);
	
	Vec3 nc, fc;

	glm::vec3 glmviewDirection = inl - inp;
	Vec3 viewDirection = Vec3(glmviewDirection.x, glmviewDirection.y, glmviewDirection.z);
	//glm::vec3 normDirection = glm::normalize(viewDirection);

	fc = p + (viewDirection * farD);
	
	ftl = fc + (u * (farHeight / 2)) - (r * (farWidth / 2));
	ftr = fc + (u * (farHeight / 2)) + (r * (farWidth / 2));
	fbl = fc - (u * (farHeight / 2)) - (r * (farWidth / 2));
	fbr = fc - (u * (farHeight / 2)) + (r * (farWidth / 2));

	nc = p + (viewDirection * nearD);

	ntl = fc + (u * (nearHeight / 2)) - (r * (nearWidth / 2));
	ntr = fc + (u * (nearHeight / 2)) + (r * (nearWidth / 2));
	nbl = fc - (u * (nearHeight / 2)) - (r * (nearWidth / 2));
	nbr = fc - (u * (nearHeight / 2)) + (r * (nearWidth / 2));

	//Vec3 dir,nc,fc,X,Y,Z;

	//Z = p - l;
	//Z.normalize();

	//X = u * Z;
	//X.normalize();

	//Y = Z * X;

	//nc = p - Z * nearD;
	//fc = p - Z * farD;

	//ntl = nc + Y * nh - X * nw;
	//ntr = nc + Y * nh + X * nw;
	//nbl = nc - Y * nh - X * nw;
	//nbr = nc - Y * nh + X * nw;

	//ftl = fc + Y * fh - X * fw;
	//ftr = fc + Y * fh + X * fw;
	//fbl = fc - Y * fh - X * fw;
	//fbr = fc - Y * fh + X * fw;

	pl[LEFT].set3Points(p,ftl,fbl);
	pl[RIGHT].set3Points(p,ftr,fbr);
}

int FrustumC::pointInFrustum(glm::vec3 &inp)
{
	Vec3 p = Vec3(inp.x, inp.y, inp.z);

	int result = INSIDE;

	float dist = pl[LEFT].distance(p);

	if (dist > 24.0f)
	{
		return OUTSIDE;
	}

	dist = pl[RIGHT].distance(p);

	if (dist < -24.0f)
	{
		return OUTSIDE;
	}

	return(result);

}