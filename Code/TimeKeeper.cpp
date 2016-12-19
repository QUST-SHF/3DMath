// TimeKeeper.cpp

#include "TimeKeeper.h"

using namespace _3DMath;

TimeKeeper::TimeKeeper( void )
{
	diolationOfTime = 1.0;
	currentTime = 0.0;
	lastTime = 0.0;
	deltaTime = 0.0;
	baseTime = 0.0;
	fixedDeltaTime = 0.0;
}

/*virtual*/ TimeKeeper::~TimeKeeper( void )
{
}

/*virtual*/ void TimeKeeper::MarkCurrentTime( void )
{
	if( baseTime == 0.0 )
		baseTime = AskSystemForCurrentTimeMilliseconds();

	currentTime = AskSystemForCurrentTimeMilliseconds() - baseTime;

	if( lastTime == 0.0 )
		lastTime = currentTime;

	deltaTime = ( currentTime - lastTime ) * diolationOfTime;
	lastTime = currentTime;

	if( fixedDeltaTime != 0.0 )
		deltaTime = fixedDeltaTime * diolationOfTime;
}

/*virtual*/ double TimeKeeper::AskSystemForCurrentTimeMilliseconds( void )
{
	double ticks = ( double )clock();
	return ticks / ( double( CLOCKS_PER_SEC ) / 1000.0 );
}

double TimeKeeper::GetCurrentTimeMilliseconds( void ) const
{
	return currentTime;
}

double TimeKeeper::GetCurrentTimeSeconds( void ) const
{
	return currentTime / 1000.0;
}

double TimeKeeper::GetDeltaTimeMilliseconds( void ) const
{
	return deltaTime;
}

double TimeKeeper::GetDeltaTimeSeconds( void ) const
{
	return deltaTime / 1000.0;
}

// TimeKeeper.cpp