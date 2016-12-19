// TimeKeeper.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class TimeKeeper;
}

class _3DMATH_API _3DMath::TimeKeeper
{
public:

	TimeKeeper( void );
	virtual ~TimeKeeper( void );

	virtual void MarkCurrentTime( void );
	virtual double AskSystemForCurrentTimeMilliseconds( void );

	double GetCurrentTimeMilliseconds( void ) const;
	double GetCurrentTimeSeconds( void ) const;
	double GetDeltaTimeMilliseconds( void ) const;
	double GetDeltaTimeSeconds( void ) const;

	double fixedDeltaTime;
	double diolationOfTime;

protected:

	double currentTime;
	double lastTime;
	double deltaTime;
	double baseTime;
};

// TimeKeeper.h