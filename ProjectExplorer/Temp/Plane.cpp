#include "Plane.h"

Plane::Plane() {}

Plane::Plane(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3)
{
	set3Points(v1, v2, v3);
}

Plane::~Plane() {}

void Plane::set3Points(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3)
{
	glm::vec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = aux2 * aux1;

	normal = glm::normalize(normal);
	point = v2;
	// normal.x * point.x + normal.y * point.y + normal.z * point.z
	d = - (glm::dot(normal, point));
}

void Plane::setNormalAndPoint(glm::vec3 &normal, glm::vec3 &point)
{
	this->normal = normal;
	this->normal = glm::normalize(this->normal);
	// this->normal.x * point.x + this->normal.y * point.y + this->normal.z * point.z
	d = - (glm::dot(this->normal, point));
}

void Plane::setCoefficients(float a, float b, float c, float d) {

	// set the normal vector
	normal = glm::vec3(a, b, c);
	//compute the lenght of the vector
	float l = normal.length();
	
	// normalize the vector
	normal = glm::vec3(a / l, b / l, c / l);
	// and divide d by th length as well
	this->d = d / l;
}

float Plane::distance(glm::vec3 &p)
{
	// (normal.x * p.x + normal.y * p.y + normal.z * p.z)
	return (d + glm::dot(normal, p));
}