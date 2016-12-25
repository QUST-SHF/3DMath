// TimeKeeper.cpp

#include "TimeKeeper.h"

using namespace _3DMath;

//-----------------------------------------------------------------------
//                               TimeKeeper
//-----------------------------------------------------------------------

TimeKeeper::TimeKeeper( void )
{
	currentTimeMilliseconds = 0.0;
	lastTimeMilliseconds = 0.0;
	deltaTimeMilliseconds = 0.0;
	baseTimeMilliseconds = 0.0;
	fixedDeltaTimeMilliseconds = 0.0;
}

/*virtual*/ TimeKeeper::~TimeKeeper( void )
{
}

/*virtual*/ void TimeKeeper::MarkCurrentTime( void )
{
	if( baseTimeMilliseconds == 0.0 )
		baseTimeMilliseconds = AskSystemForCurrentTimeMilliseconds();

	currentTimeMilliseconds = AskSystemForCurrentTimeMilliseconds() - baseTimeMilliseconds;

	if( lastTimeMilliseconds == 0.0 )
		lastTimeMilliseconds = currentTimeMilliseconds;

	deltaTimeMilliseconds = currentTimeMilliseconds - lastTimeMilliseconds;
	lastTimeMilliseconds = currentTimeMilliseconds;
}

/*virtual*/ double TimeKeeper::AskSystemForCurrentTimeMilliseconds( void )
{
	double ticks = ( double )clock();
	return ticks / ( double( CLOCKS_PER_SEC ) / 1000.0 );
}

double TimeKeeper::GetCurrentTimeMilliseconds( void ) const
{
	return currentTimeMilliseconds;
}

double TimeKeeper::GetCurrentTimeSeconds( void ) const
{
	return GetCurrentTimeMilliseconds() / 1000.0;
}

double TimeKeeper::GetDeltaTimeMilliseconds( void ) const
{
	if( fixedDeltaTimeMilliseconds != 0.0 )
		return fixedDeltaTimeMilliseconds;
	
	return deltaTimeMilliseconds;
}

double TimeKeeper::GetDeltaTimeSeconds( void ) const
{
	return GetDeltaTimeMilliseconds() / 1000.0;
}

// TimeKeeper.cpp