#include "FrustumC.h"

#define ANG2RAD 3.14159265358979323846/180.0

FrustumC::FrustumC()
{
}

FrustumC::~FrustumC()
{
}

void FrustumC::setCamInternals(float angle, float ratio, float nearDistance, float farDistance, Camera* camera)
{
	// store the information
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearDistance;
	this->farD = farDistance;
	this->camera = camera;

	// compute width and height of the near and far plane sections
	tang = (float)tan(ANG2RAD * angle * 0.5);
	nh = nearDistance * tang;
	nw = nh * ratio;
	fh = farDistance  * tang;
	fw = fh * ratio;
}

void FrustumC::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u, float projRatio)
{
	glm::vec3 nc, fc;

	glm::vec3 viewDirection = l - p;
	glm::vec3 normDirection = glm::normalize(viewDirection);

	GLfloat farHeight = 2 * std::tan(camera->Zoom / 2) *  farD;
	GLfloat farWidth = farHeight  * projRatio;
	GLfloat nearHeight = 2 * std::tan(camera->Zoom / 2) *  nearD;
	GLfloat nearWidth = farHeight  * projRatio;

	GLfloat maxRadious = (farWidth / farHeight >= 1) ? farWidth : farHeight;

	fc = camera->Position + (viewDirection * farD);

	ftl = fc + (camera->Up * (farHeight / 2)) - (camera->Right * (farWidth / 2));
	ftr = fc + (camera->Up * (farHeight / 2)) + (camera->Right * (farWidth / 2));
	fbl = fc - (camera->Up * (farHeight / 2)) - (camera->Right * (farWidth / 2));
	fbr = fc - (camera->Up * (farHeight / 2)) + (camera->Right * (farWidth / 2));

	nc = camera->Position + (viewDirection * nearD);
	
	ntl = fc + (camera->Up * (nearHeight / 2)) - (camera->Right * (nearWidth / 2));
	ntr = fc + (camera->Up * (nearHeight / 2)) + (camera->Right * (nearWidth / 2));
	nbl = fc - (camera->Up * (nearHeight / 2)) - (camera->Right * (nearWidth / 2));
	nbr = fc - (camera->Up * (nearHeight / 2)) + (camera->Right * (nearWidth / 2));

	//glm::vec3 dir, nc, fc, X, Y, Z;

	//// compute the Z axis of camera
	//// this axis points in the opposite direction from
	//// the looking direction
	//Z = l - p;
	//glm::normalize(Z);
	////Z.normalize();

	//// X axis of camera with given "up" vector and Z axis
	//X = u * Z;
	//glm::normalize(X);
	////X.normalize();

	//// the real "up" vector is the cross product of Z and X
	//Y = Z * X;

	//// compute the centers of the near and far planes
	//nc = p - Z * nearD;
	//fc = p - Z * farD;

	//// compute the 4 corners of the frustum on the near plane
	//ntl = nc + Y * nh - X * nw;
	//ntr = nc + Y * nh + X * nw;
	//nbl = nc - Y * nh - X * nw;
	//nbr = nc - Y * nh + X * nw;

	//// compute the 4 corners of the frustum on the far plane
	//ftl = fc + Y * fh - X * fw;
	//ftr = fc + Y * fh + X * fw;
	//fbl = fc - Y * fh - X * fw;
	//fbr = fc - Y * fh + X * fw;

	// compute the six planes
	// the function set3Points assumes that the points
	// are given in counter clockwise order
	planes[LEFT].set3Points(ntl, nbl, fbl);
	planes[RIGHT].set3Points(nbr, ntr, fbr);
}

int FrustumC::pointInFrustum(glm::vec3 &p) {

	int result = INSIDE;

	for (int i = 0; i < 2; i++)
	{
		float dist = planes[i].distance(p);

		if (dist < 0)
		{
			return OUTSIDE;
		}
	}
	return(result);
}