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
}