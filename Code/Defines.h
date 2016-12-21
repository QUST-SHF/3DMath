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
#define PHI				1.6180339887

#define MIN(a,b)		((a) < (b) ? (a) : (b))
#define MAX(a,b)		((a) < (b) ? (b) : (a))

#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif

// TODO: Get rid of these includes and place them where appropriate.
#include <math.h>
#include <stdint.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <time.h>

// Defines.h