// Defines.h

#pragma once

#if defined _3DMATH_API_EXPORT
#	define _3DMATH_API		__declspec( dllexport )
#elif defined _3DMATH_API_IMPORT
#	define _3DMATH_API		__declspec( dllimport )
#else
#	define _3DMATH_API
#endif

#define EPSILON			1e-6

#define _USE_MATH_DEFINES

#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <string>

// Defines.h