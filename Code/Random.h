// Random.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class Random;
	class Vector;
}

class _3DMATH_API _3DMath::Random
{
public:

	Random( void );
	~Random( void );

	void Seed( int seed );

	int Integer( int min, int max );
	double Float( double min, double max );
	void VectorInCone( const Vector& unitAxis, double coneAngle, Vector& randomVector );
};

// Random.h
