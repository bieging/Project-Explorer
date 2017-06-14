#ifndef BMATH_H
#define BMATH_H

//#pragma once

#include <string>
#include <limits>

// This is my math library, or Bieging Math

namespace bmath
{
	// todo - create comment for this
	float norm(float value, float start, float stop);

	// todo - create comment for this
	float map(float value,
		float start1, float stop1,
		float start2, float stop2);
}

#endif // !BMATH_H