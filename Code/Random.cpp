// Random.cpp

#include "Random.h"
#include "Vector.h"

using namespace _3DMath;

Random::Random( void )
{
}

Random::~Random( void )
{
}

void Random::Seed( int seed )
{
	// Unfortunately, this seeds the random number generator for all instances.
	srand( ( unsigned )seed );
}

int Random::Integer( int min, int max )
{
	//int randomInt = ( int )round( Float( min, max ) );
	int randomInt = ( int )floor( Float( min, max + 1 ) );
	if( randomInt < min )
		randomInt = min;
	if( randomInt > max )
		randomInt = max;
	return randomInt;
}

double Random::Float( double min, double max )
{
	double randomFloat = min + double( rand() ) / double( RAND_MAX ) * ( max - min );
	return randomFloat;
}

void Random::VectorInCone( const Vector& unitAxis, double coneAngle, Vector& randomVector )
{
	double angle = Float( 0.0, coneAngle );
	//...
}

void Random::VectorInBox( const AxisAlignedBox& box, Vector& randomVector )
{
}

void Random::VectorInInterval( double min, double max, Vector& randomVector )
{
	randomVector.x = Float( min, max );
	randomVector.y = Float( min, max );
	randomVector.z = Float( min, max );
}

// Random.cpp