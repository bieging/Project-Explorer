#include "BMath.h"

namespace bmath
{
	// todo - create comment for this
	float norm(float value, float start, float stop)
	{
		return (value - start) / (stop - start);
	}

	// todo - create comment for this
	float map(float value,
		float start1, float stop1,
		float start2, float stop2)
	{
		return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
	}

	float distPT2LN(glm::vec3 linePoint, glm::vec3 lineVector, glm::vec3 point)
	{
		glm::vec3 u = point - linePoint;
		glm::vec3 puv = ((u * lineVector) / glm::abs(lineVector)) * lineVector;
		glm::vec3 q_ = linePoint + puv;
		return glm::distance(point, q_);
	}
}